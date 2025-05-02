//  Created by Leo Bellantoni on 4/1/23.



#include <numeric>



namespace BdlHisto {

    void plotBdlIntegral(FieldMap* fieldMap) {
    
        FieldMap::XYbinning scanOver = fieldMap->getXYbinning();
        Lbins xBins(scanOver.nXbins, scanOver.xLow, scanOver.xHigh);
        Lbins yBins(scanOver.nYbins, scanOver.yLow, scanOver.yHigh);
        Lhist2 hBdlHisto(xBins,yBins,
                    "{/Symbol \362}{/Palatino:Bold B}y{/Symbol \267}{/Palatino:Italic dl} [T m]");


        int nQuBins;    double loQuBins;
        if (scanOver.nXbins%2==1) {
            nQuBins  = (scanOver.nXbins+1)/2;
            loQuBins = (scanOver.xHigh -scanOver.xLow) / (2*scanOver.nXbins);
        } else {
            nQuBins  = scanOver.nXbins/2;
            loQuBins = 0.0;
        }
        Lbins quBins(nQuBins, -loQuBins, scanOver.yHigh);
        Lhist2 hQuBdlAbs(quBins,quBins,
                    "Variation in {/Symbol \362}{/Palatino:Bold B}y{/Symbol \267}{/Palatino:Italic dl} [T m]");
        Lhist2 hQuBdlRel(quBins,quBins,
                    "Relative variation in {/Symbol \362}{/Palatino:Bold B}y{/Symbol \267}{/Palatino:Italic dl}");

        vector<double> zValues = fieldMap->getZgrid();

        FieldMap::FieldMapInd ind;      ind.iZ = 0;
        for (ind.iX=0; ind.iX<scanOver.nXbins; ++ind.iX) {
            for (ind.iY=0; ind.iY<scanOver.nYbins; ++ind.iY) {
                ra<FieldMap::AtPoint> lineInZ = fieldMap->sameXY(ind);
                double integral = 0.0;
                if (lineInZ(0).valid) {
                    // Actually, it IS your grampa's integral
                    for (int iIng=0; iIng<lineInZ.ne1-1; ++iIng) {
                        double dIntegral  = lineInZ(iIng+1).By +lineInZ(iIng).By;
                               dIntegral /= 2.0;
                        dIntegral *=(lineInZ(iIng+1).z -lineInZ(iIng).z);
                        integral += dIntegral;
                    }
                }

                FieldMap::FieldMapItr itr;
                fieldMap->findItr(ind, itr);
                if (integral!=0) hBdlHisto.Fill(itr->x,itr->y,integral);
            }
        }

        Lzcol pBdlHisto(hBdlHisto);
        pBdlHisto.blankEmpties();
        pBdlHisto.show();

        vector<double> values;
        for (int iBinX=scanOver.nXbins -nQuBins +1; iBinX<=scanOver.nXbins; ++iBinX) {
            double absX = hBdlHisto.GetXCenter(iBinX);
            for (int iBinY=scanOver.nXbins -nQuBins +1; iBinY<=scanOver.nYbins; ++iBinY) {
                double absY = hBdlHisto.GetYCenter(iBinY);
                if (hBdlHisto.GetBinPos(absX,absY).virgin) continue;

                values.clear();
                values.push_back(hBdlHisto.GetBinPos(+absX,+absY).GetValue());
                values.push_back(hBdlHisto.GetBinPos(+absX,-absY).GetValue());
                values.push_back(hBdlHisto.GetBinPos(-absX,+absY).GetValue());
                values.push_back(hBdlHisto.GetBinPos(-absX,-absY).GetValue());
                double minV = *(std::min_element(values.begin(),values.end()));
                double maxV = *(std::max_element(values.begin(),values.end()));
                double aveV = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
                
                hQuBdlAbs.Fill(absX,absY, abs(maxV-minV));
                hQuBdlRel.Fill(absX,absY, abs((maxV-minV)/aveV));
            }
        }

        Lzcol pQuBdlAbs(hQuBdlAbs);
        pQuBdlAbs.XlowAuto  = false;        pQuBdlAbs.XlowLim  = -loQuBins;
        pQuBdlAbs.XhighAuto = false;        pQuBdlAbs.XhighLim = scanOver.xHigh;
        pQuBdlAbs.YlowAuto  = false;        pQuBdlAbs.YlowLim  = -loQuBins;
        pQuBdlAbs.YhighAuto = false;        pQuBdlAbs.YhighLim = scanOver.yHigh;
        pQuBdlAbs.blankEmpties();
        pQuBdlAbs.show();

        Lzcol pQuBdlRel(hQuBdlRel);
        pQuBdlRel.XlowAuto  = false;        pQuBdlRel.XlowLim  = -loQuBins;
        pQuBdlRel.XhighAuto = false;        pQuBdlRel.XhighLim = scanOver.xHigh;
        pQuBdlRel.YlowAuto  = false;        pQuBdlRel.YlowLim  = -loQuBins;
        pQuBdlRel.YhighAuto = false;        pQuBdlRel.YhighLim = scanOver.yHigh;
        pQuBdlRel.blankEmpties();
        pQuBdlRel.show();
        
        int nBinsX = hQuBdlRel.GetXNbins(); int nBinsY = hQuBdlRel.GetYNbins();
        cout<<
            "Largest value of (max(integral) - min(integral)) / mean(integral) over the 4 fold symmetry is "
            << hQuBdlRel.MaxVal() << "; and average value of same is " <<
            hQuBdlRel.SumBins(1,nBinsX, 1,nBinsY).GetValue() / (nBinsX*nBinsY) <<endl;
            
    }
}
