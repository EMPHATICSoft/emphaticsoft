#ifndef ALIGNMENT_KALIGNPARAMETERS_H
#define ALIGNMENT_KALIGNPARAMETERS_H

#include <array>
#include <map>
#include <string>
#include <tuple>
#include <vector>

namespace emph {
namespace alignment {

  // Alignment degrees of freedom. Per SSD sensor: Tn is the translation
  // normal to the strips, Tpar along the strips, Tz along the beam, and Rz
  // the rotation about the sensor center. The magnet has the global
  // translations FieldX and FieldY of the field map.
  enum class DOF { Tn = 0, Tpar = 1, Tz = 2, Rz = 3, FieldX = 4, FieldY = 5 };
  constexpr int kNDOF = 6;
  constexpr int kNSensorDOF = 4;
  constexpr int kMagnetStation = 9999;

  const char* DOFName(DOF dof);

  struct SensorKey {
    int station = 0;
    int plane = 0;
    int sensor = 0;
    bool operator<(const SensorKey& o) const
    {
      return std::tie(station, plane, sensor) <
             std::tie(o.station, o.plane, o.sensor);
    }
    bool operator==(const SensorKey& o) const
    {
      return station == o.station && plane == o.plane && sensor == o.sensor;
    }
  };

  // Pseudo-sensor key that holds the magnet (field) parameters.
  inline SensorKey MagnetKey() { return SensorKey{kMagnetStation, 0, 0}; }

  // Maps (sensor, DOF) to the index in the vector of free alignment
  // parameters. Fixed parameters have no index and no covariance.
  class KAlignParameters {
  public:
    // Prior standard deviations (mm for translations, rad for Rz) applied to
    // every free parameter of the given DOF type; prior means are zero.
    void SetPriorSigma(DOF dof, double sigma);
    double PriorSigma(DOF dof) const { return fPriorSigma[static_cast<int>(dof)]; }

    void AddSensor(const SensorKey& key);
    // Adds the magnet parameters (FieldX, FieldY) under MagnetKey().
    void AddMagnet();

    // Fix parameters by a spec "station:plane:sensor:dof". Each of the first
    // three fields is an integer or '*'; dof is Tn, Tpar, Tz, Rz or '*'.
    // Throws std::invalid_argument on a malformed spec or if it matches no
    // known sensor. Wildcard specs never match the magnet; fix its parameters
    // with "magnet:FieldX", "magnet:FieldY" or "magnet:*". Call after all
    // sensors (and the magnet) are added.
    void Fix(const std::string& spec);
    void Fix(const SensorKey& key, DOF dof);

    // Assigns indices to free parameters in (sensor, DOF) order. Must be
    // called after all AddSensor/Fix calls and before the queries below.
    void Finalize();

    bool IsFixed(const SensorKey& key, DOF dof) const;
    // Index among free parameters, or -1 if fixed or unknown.
    int Index(const SensorKey& key, DOF dof) const;
    int NFree() const { return static_cast<int>(fFree.size()); }
    SensorKey Key(int index) const { return fFree.at(index).first; }
    DOF GetDOF(int index) const { return fFree.at(index).second; }
    std::string Label(int index) const;

    std::vector<double> PriorMean() const;
    std::vector<std::vector<double>> PriorCovariance() const;

  private:
    std::array<double, kNDOF> fPriorSigma{{1.0, 1.0, 1.0, 0.01, 1.0, 1.0}};
    std::map<SensorKey, std::array<bool, kNDOF>> fFixed;
    std::vector<std::pair<SensorKey, DOF>> fFree;
    std::map<SensorKey, std::array<int, kNDOF>> fIndex;
    bool fFinalized = false;
  };

} // namespace alignment
} // namespace emph

#endif
