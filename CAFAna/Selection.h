bool IsGoodBeamTrack(const SRTrack& trk)
{
  // beam track must have no more than 4 SSD clusters, one per plane.
  return false;
}

bool HasGoodChi2(const SRTrack& trk)
{
  return (trk.chi2 < kChi2Cut);
}

bool HasOkMomemtum(const SRTrack& trk)
{
  return (trk.ptot > kPTotMin && trk.ptot < kPTotMax);
}

bool HasOkARICHPID(const SRTrack& trk)
{
  return ( (trk.arich.score[kBeta1] > kARICHScoreMin) &&
        (trk.arich.score[kPion] > kARICHScoreMin) &&
    (trk.arich.score[kKaon] > kARICHScoreMin) &&
    (trk.arich.score[kProton] > kARICHScoreMin) );
}

bool HasOkVertex(const SRVertex& vtx)
{
    return ( ( vtx.x > kVtxXMin && vtx.x < kVtxXMax) &&
      ( vtx.y > kVtxYMin && vtx.y < kVtxYMax) &&
      ( vtx.z > kVtxZMin && vtx.z < kVtxZMax) );
}

