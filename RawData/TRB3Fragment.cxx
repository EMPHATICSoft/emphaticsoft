#include "emphatic-artdaq/Overlays/TRB3Fragment.hh"

std::ostream& emphaticdaq::operator<<(std::ostream& os, TRB3Fragment const& f)
{
  os << "TRB3Fragment: sz=" << f.subEventCount()  << "\n";

	return os;
}

size_t emphaticdaq::TRB3Fragment::subEventCount() const {
  size_t count = 0;
  auto subevt_ptr = reinterpret_cast<uint8_t const*>(dataBegin() + 1);
  while(subevt_ptr != artdaq_Fragment_.dataEndBytes()) {
    ++count;
    subevt_ptr += reinterpret_cast<TRB3SubEventHeader const*>(subevt_ptr)->subevent_size;
  }

  return count;
}

emphaticdaq::TRB3Fragment::TRB3SubEventHeader const* emphaticdaq::TRB3Fragment::subEventIndex(size_t idx) const {
  if(idx > subEventCount() - 1) {
    TLOG(TLVL_ERROR) << "Index out of range! idx=" << idx << ", max=" << subEventCount() - 1;
    return nullptr;
  }

  size_t count = 0;
  auto subevt_ptr = reinterpret_cast<uint8_t const*>(dataBegin() + 1);
  while(count < idx) {
    ++count;
    subevt_ptr += reinterpret_cast<TRB3SubEventHeader const*>(subevt_ptr)->subevent_size;
  }

  return reinterpret_cast<TRB3SubEventHeader const*>(subevt_ptr);
}
