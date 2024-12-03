//  Created by Leo Bellantoni on 4/1/23.



#include "plusORminus.h"
#include "meansig.h"

namespace divHists {



    void plotDivergenceHistograms(FieldMap* cnMap, FieldMap* upMap, FieldMap* dnMap) {

        Lbins  bDivCenter(nBinsHistoCenter, loHistoEdgeCenter, hiHistoEdgeCenter);
        Lbins  bDivFringe(nBinsHistoCenter, loHistoEdgeFringe, hiHistoEdgeFringe);
        Lhist1 hDivCentral   (bDivCenter,"{/Symbol \321\267}{/Palatino:Bold B} [T/m] - central field map");
        Lhist1 hDivUpstream  (bDivFringe,"{/Symbol \321\267}{/Palatino:Bold B} [T/m] - upstream field map");
        Lhist1 hDivDownstream(bDivFringe,"{/Symbol \321\267}{/Palatino:Bold B} [T/m] - downstream field map");
        meansig divStats;



        if (cnMap!=nullptr) {
            for (FieldMap::FieldMapItr itr=cnMap->pBegin; itr<cnMap->pEnd; ++itr) {
                FieldMap::FieldMapInd ind;
                if (!cnMap->findInd(itr,ind)) continue;

                double div = itr->valid ? cnMap->findDiv(ind) : Nana;
                if (div!=Nana) {
                    divStats.push(div);
                    hDivCentral.Fill(div);
                }
            }
            Lfit1 pDivCentral(hDivCentral);
            pDivCentral.logY = logPlots;
            pDivCentral.show();
            cout << "Mean divergence in central field is       " <<
                plusORminus::formatted(divStats.mean(),divStats.mean_err(),plusORminus::milli,"T/m") <<
                " & width is " << divStats.stdev() << " T/m" << endl;
        }



        if (upMap!=nullptr) {
            divStats.reset();
            for (FieldMap::FieldMapItr itr=upMap->pBegin; itr<upMap->pEnd; ++itr) {
                FieldMap::FieldMapInd ind;
                if (!upMap->findInd(itr,ind)) continue;
                double div  = upMap->findDiv(ind);
                if (div!=Nana) {
                    divStats.push(div);
                    hDivUpstream.Fill(div);
                }
            }
            Lfit1 pDivUpstream(hDivUpstream);
            pDivUpstream.logY = logPlots;
            pDivUpstream.show();
            cout << "Mean divergence in upstream field is      " <<
                plusORminus::formatted(divStats.mean(),divStats.mean_err(),plusORminus::milli,"T/m") <<
                " & width is " << divStats.stdev() << " T/m" << endl;
        }



        if (dnMap!=nullptr) {
            divStats.reset();
            for (FieldMap::FieldMapItr itr=dnMap->pBegin; itr<dnMap->pEnd; ++itr) {
                FieldMap::FieldMapInd ind;
                if (!dnMap->findInd(itr,ind)) continue;
                double div  = dnMap->findDiv(ind);
                if (div!=Nana) {
                    divStats.push(div);
                    hDivDownstream.Fill(div);
                }
            }
            Lfit1 pDivDownstream(hDivDownstream);
            pDivDownstream.logY = logPlots;
            pDivDownstream.show();
            cout << "Mean divergence in downstream field is    " <<
                plusORminus::formatted(divStats.mean(),divStats.mean_err(),plusORminus::milli,"T/m") <<
                " & width is " << divStats.stdev() << " T/m" << endl;
        }



        return;
    }
}
