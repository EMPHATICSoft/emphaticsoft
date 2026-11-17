#include "Alignment/Kalman/KAlignParameters.h"

#include <cmath>
#include <sstream>
#include <stdexcept>

namespace emph {
namespace alignment {

  const char* DOFName(DOF dof)
  {
    switch (dof) {
    case DOF::Tn: return "Tn";
    case DOF::Tpar: return "Tpar";
    case DOF::Tz: return "Tz";
    case DOF::Rz: return "Rz";
    case DOF::FieldX: return "FieldX";
    case DOF::FieldY: return "FieldY";
    }
    return "?";
  }

  namespace {
    // Parses an integer or '*' (returned as wildcard = true).
    int ParseField(const std::string& field, bool& wildcard,
                   const std::string& spec)
    {
      wildcard = (field == "*");
      if (wildcard) return 0;
      size_t pos = 0;
      int value = 0;
      try {
        value = std::stoi(field, &pos);
      }
      catch (const std::exception&) {
        pos = 0;
      }
      if (field.empty() || pos != field.size())
        throw std::invalid_argument("Bad fixed-parameter spec: " + spec);
      return value;
    }
  }

  void KAlignParameters::SetPriorSigma(DOF dof, double sigma)
  {
    if (!(sigma > 0.0) || !std::isfinite(sigma))
      throw std::invalid_argument("Prior sigma must be positive and finite");
    fPriorSigma[static_cast<int>(dof)] = sigma;
  }

  void KAlignParameters::AddSensor(const SensorKey& key)
  {
    if (fFinalized)
      throw std::logic_error("KAlignParameters already finalized");
    fFixed.emplace(key, std::array<bool, kNDOF>{{false, false, false, false,
                                                  true, true}});
  }

  void KAlignParameters::AddMagnet()
  {
    if (fFinalized)
      throw std::logic_error("KAlignParameters already finalized");
    fFixed.emplace(MagnetKey(), std::array<bool, kNDOF>{{true, true, true, true,
                                                          false, false}});
  }

  void KAlignParameters::Fix(const SensorKey& key, DOF dof)
  {
    if (fFinalized)
      throw std::logic_error("KAlignParameters already finalized");
    auto it = fFixed.find(key);
    if (it == fFixed.end())
      throw std::invalid_argument("Cannot fix parameter of unknown sensor");
    it->second[static_cast<int>(dof)] = true;
  }

  void KAlignParameters::Fix(const std::string& spec)
  {
    if (fFinalized)
      throw std::logic_error("KAlignParameters already finalized");
    std::vector<std::string> fields;
    std::stringstream ss(spec);
    std::string item;
    while (std::getline(ss, item, ':')) fields.push_back(item);

    auto parseDofs = [&spec](const std::string& field) {
      std::vector<DOF> dofs;
      if (field == "*") {
        for (int i = 0; i < kNDOF; ++i) dofs.push_back(static_cast<DOF>(i));
        return dofs;
      }
      for (int i = 0; i < kNDOF; ++i)
        if (field == DOFName(static_cast<DOF>(i)))
          dofs.push_back(static_cast<DOF>(i));
      if (dofs.empty())
        throw std::invalid_argument("Bad DOF in fixed-parameter spec: " + spec);
      return dofs;
    };

    if (fields.size() == 2 && fields[0] == "magnet") {
      auto it = fFixed.find(MagnetKey());
      if (it == fFixed.end())
        throw std::invalid_argument("Fixed-parameter spec but no magnet: " +
                                    spec);
      for (DOF d : parseDofs(fields[1])) it->second[static_cast<int>(d)] = true;
      return;
    }

    if (fields.size() != 4)
      throw std::invalid_argument("Bad fixed-parameter spec: " + spec);

    bool wStation, wPlane, wSensor;
    const int station = ParseField(fields[0], wStation, spec);
    const int plane = ParseField(fields[1], wPlane, spec);
    const int sensor = ParseField(fields[2], wSensor, spec);
    const std::vector<DOF> dofs = parseDofs(fields[3]);

    bool matched = false;
    for (auto& entry : fFixed) {
      const SensorKey& k = entry.first;
      if (k.station == kMagnetStation) continue;
      if ((wStation || k.station == station) && (wPlane || k.plane == plane) &&
          (wSensor || k.sensor == sensor)) {
        matched = true;
        for (DOF d : dofs) entry.second[static_cast<int>(d)] = true;
      }
    }
    if (!matched)
      throw std::invalid_argument("Fixed-parameter spec matches no sensor: " +
                                  spec);
  }

  void KAlignParameters::Finalize()
  {
    fFree.clear();
    fIndex.clear();
    for (const auto& entry : fFixed) {
      std::array<int, kNDOF> indices;
      indices.fill(-1);
      for (int d = 0; d < kNDOF; ++d) {
        if (entry.second[d]) continue;
        indices[d] = static_cast<int>(fFree.size());
        fFree.emplace_back(entry.first, static_cast<DOF>(d));
      }
      fIndex[entry.first] = indices;
    }
    fFinalized = true;
  }

  bool KAlignParameters::IsFixed(const SensorKey& key, DOF dof) const
  {
    auto it = fFixed.find(key);
    return it == fFixed.end() || it->second[static_cast<int>(dof)];
  }

  int KAlignParameters::Index(const SensorKey& key, DOF dof) const
  {
    if (!fFinalized)
      throw std::logic_error("KAlignParameters not finalized");
    auto it = fIndex.find(key);
    return it == fIndex.end() ? -1 : it->second[static_cast<int>(dof)];
  }

  std::string KAlignParameters::Label(int index) const
  {
    const auto& p = fFree.at(index);
    std::ostringstream os;
    if (p.first.station == kMagnetStation) {
      os << "magnet_" << DOFName(p.second);
      return os.str();
    }
    os << "st" << p.first.station << "_pl" << p.first.plane << "_sn"
       << p.first.sensor << "_" << DOFName(p.second);
    return os.str();
  }

  std::vector<double> KAlignParameters::PriorMean() const
  {
    return std::vector<double>(fFree.size(), 0.0);
  }

  std::vector<std::vector<double>> KAlignParameters::PriorCovariance() const
  {
    std::vector<std::vector<double>> cov(
      fFree.size(), std::vector<double>(fFree.size(), 0.0));
    for (size_t i = 0; i < fFree.size(); ++i) {
      const double s = fPriorSigma[static_cast<int>(fFree[i].second)];
      cov[i][i] = s * s;
    }
    return cov;
  }

} // namespace alignment
} // namespace emph
