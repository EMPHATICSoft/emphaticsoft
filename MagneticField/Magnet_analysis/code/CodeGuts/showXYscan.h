//  Created by Leo Bellantoni on 4/1/23.




namespace showXYscans {



    void plotXYscanPoints(FieldMap* cnMap, FieldMap* upMap, FieldMap* dnMap) {
        Ldata::Point doink;

        vector<std::pair<double,double>> bXYCentral = cnMap->getXYgrid();
        Ldata hXYCentral   ("{/Palatino:Bold (x,y)} of central field map scans");
        for (std::pair<double,double> iPoint : bXYCentral) {
            doink.x = iPoint.first;     doink.y = iPoint.second;
            hXYCentral.push(doink);
        }

        vector<std::pair<double,double>> bXYUpstream = upMap->getXYgrid();
        Ldata hXYUpstream  ("{/Palatino:Bold (x,y)} of upstream field map scans");
        for (std::pair<double,double> iPoint : bXYUpstream) {
            doink.x = iPoint.first;     doink.y = iPoint.second;
            hXYUpstream.push(doink);
        }

        vector<std::pair<double,double>> bXYDownstream = dnMap->getXYgrid();
        Ldata hXYDownstream("{/Palatino:Bold (x,y)} of downstream field map scans");
        for (std::pair<double,double> iPoint : bXYDownstream) {
            doink.x = iPoint.first;     doink.y = iPoint.second;
            hXYDownstream.push(doink);
        }



        double dotSize = 1.0;
        Lscatter pXYCentral(hXYCentral);
        pXYCentral.SetDotSize(dotSize);
        pXYCentral.show();

        Lscatter pXYUpstream(hXYUpstream);
        pXYUpstream.SetDotSize(dotSize);
        pXYUpstream.show();

        Lscatter pXYDownstream(hXYDownstream);
        pXYDownstream.SetDotSize(dotSize);
        pXYDownstream.show();



        return;
    }
}
