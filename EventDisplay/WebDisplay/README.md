# EMPHATIC Web Display
A browser-based event display for the EMPHATIC experiment.

## Usage
1. Log in to an emphaticgpvm with a **special ssh flag**: `ssh -L 3490:localhost:3490 emphaticgpvm03.fnal.gov`
2. Set up to develop emphaticsoft normally.  You'll forward port 3490 through the SL7 container since you're already on this branch.
3. Find a file you want to read.  It must have **SSD LineSegments** from the MakeSSDClusters module since that's all the event display shows.
4. Start the "web server" ART job: `art -c webDisplay.fcl <yourFile.root>`
5. Open a web browser **on your desktop**.  Type into the URL bar: `localhost:3490`
6. The browser make take a few seconds to load the first event.  Then you should see a black screen with teal lines and some blue and red shapes:
  - Teal lines are SSD LineSegments
  - Red shapes are the magnet and the target
  - Blue shapes are the outlines of SSD sensors
7. How to interact with the browser:
  - Left click and drag to change your viewing angle
  - Right click and drag to change your viewing position in the plane you're viewing from
  - Scroll to zoom in or out
  - Reload the page to go to the next event
  - Double click to "focus" the camera on an object
  - Reload the web page to reset the camera

## Supported Platforms
- Your laptop (client side):
  - Firefox on Ubuntu (TODO: version.  Mine is pretty recent)
  - Chrome on Ubuntu (TODO: version.  Mine is very old)
  - Chrome on OS X
  - TODO: Safari on OS X?
- Server side:
  - Any POSIX OS on which you can compile emphaticsoft

## Details
Might be useful in case you need to debug something

- The event display just draws LineSegments and MC trajectories right now.  No tracks, hits, or Cherenkov information (yet!).  The magnet and target positions are from the Geometry service.  The magnet geometry is the only geometry component loaded directly from the GDML file right now.  Ask if you want more volumes like this.  I'm trying to limit detailed geometry to keep the focus on the physics.
- The WebDisplay is acting as a trivial "web server" that sends and receives HTTP over a port.  It now figures out when to go to the next event by counting requests.  I get 1 request per magnet segment + 1 request to go to the new event.  You'll get a bunch of numbers on your screen if it gets out of sync somehow.  Please tell me if that happens so I can fix it!
- You have to get port 3490 from the ART job to your web browser on your device.  In these instructions, it is forwarded by ssh which should even encrypt traffic over the port and requires you to authenticate using kerberos to use it effectively.  Not so friendly to non-collaborators, but hopefully overly safe since I'm a web neophyte.
- The 3D display is run client-side (on your computer, not the GPVM) using a Javascript library called three.js.  I got the idea to use this library from CMS's outreach event display.
- The GPVM "web server" is just a code generator that fills in some Javascript based on the LineSegments in the display and the Geometry service.
- We require a POSIX OS (i.e. not Windows) for 2 reasons that I know of: the sockets API, and the stat() API.  stat() is easy to replace with ROOT shenanigans that I don't like from TSystem.  ROOT and Boost may have more robust socket implementations.  The only reason I didn't use a portable socket implementation was to save myself work on the demonstration.

## TODOs
- Before next release:
  - New data product to override object colors with ART Assns to that product
    - New data product, evd::Metadata
    - To detect Assns without skipping unassociated products, do I put everything into a map and then loop over Assns<> to override?  I could also do some Assns magic that Jon put in AssnsUtils.
    - Demonstration: Write a module that tags e.g. some LineSegments but not others.  Point out that this could be done **in a Producer module** and that evd::Metadata products could be dropped for production from the FHICL file.
  - Why doesn't this work on Safari?  Is it some security setting?  Should we disable the setting or change how the application works?
  - Test on just data and just MC
- Before next collaboration meeting:
  - Hand these off to someone else to test maintainability!
    - Include SSD Tracks and TrackSegments.  Robert has these working now!
    - Include the ARICH.  Draw boxes for the ARICH hits?  Paste the 2D histogram on the front of the box as a texture?
  - How do we manage port numbers with many users on the same GPVM?  Right now, they see each other's event displays!
  - Run under valgrind
- Later...
  - "Light mode" background for creating slides
  - Add points where MC particles scattered?
  - "Error" text on the screen when the Javascript gets stuck
  - Port this to a real web server running e.g. at Notre Dame?
    - That would be nice for outreach!
    - But then someone has to understand and maintain a web server application.
    - The webserver I'm imagining would produce a web page that looks like Nathaniel Tagg's Arachne event display for MINERvA.  We could make an ART job act like a "CGI script" by reading requests on STDIN and replying on STDOUT.  It even sounds like we could leave the ART job running between serving events with Apache.  When I searched for ideas using Google, I saw recommendations that we might as well write an Apache module if we're forced to use C.  I'm not sure how to do that, and it sounds like more dependencies to me.
    - Better yet, deliver a container that runs this thing through a web server.  Then we could deploy it anywhere we need to as a backup plan, and I could test it easiy.
  - Make sure rotations from alignment procedure eventually make it into the event display.  Someone will have to translate measured rotation angles into a 4x4 "homogenous matrix"
  - How often do we have to update the Javascript side of this application?  How often does the THREE.js API change?  Not work with certain browsers?  As a Javascript newcomer, I don't have easy answers to these questions.  CMS made it work somehow, so I have some hope this won't be too bad...
  - Could I refactor EventLoader to use `THREE.DefaultLoadingManager` to trigger `render()`?
    - Main advantage: No more `await` and `Promise`!  Just add your function call to the main `requestNewEvent()` function.  Order doesn't matter anymore.  Much simpler to maitain.
    - Problem: All `fetch()`es have to go through `THREE.FileLoader`.  That means I *can't use POST* anymore.  I'd have to refactor my "web server" a bit.  Can I even provide a body in GET requests?

## v3
- Display parameter sets for double clicked objects on demand.  This also lays the groundwork for displaying Assns<> relationships by uploading ProductIDs to the front-end.
  - How to get parameter set that produced an arbitary product in ART!
  - Event is a ProductRetriever: https://github.com/art-framework-suite/art/blob/develop/art/Framework/Principal/Event.h
  - ProductRetriever can get Provenance by a ProductID: https://github.com/art-framework-suite/art/blob/develop/art/Framework/Principal/ProductRetriever.h
  - Provenance provides all of the goodies like the ParameterSet that produced an object and its parents' ProductIDs
  - ProductID is just a number, and I can even get that number directly as an integer!  https://github.com/art-framework-suite/canvas/blob/develop/canvas/Persistency/Provenance/ProductID.h
- Display Assns<> relationships between objects
  - Key is that Assns<> give two Ptr<>s which both contain ProductIDs
  - User interface: blobs connected by arrows in a separate HTML inset.  Click on a blob to highlight its children and see its ParameterSet.  I don't know what library to use for this yet.
  - Save ProductID in either `Object3D.userData` or make it the object ID somehow.  Then, *I can make a query for the ParameterSet of an arbitrary object*!  Write a function that formats this as simple HTML.  Seems like a very powerful debugging tool to me!
  - Also note that `art::Assns<>` provide access to an `art::Ptr<>` that can provide a `ProductID`.  So I could in principle extract a map of ProductIDs that are associated and have Javascript build a scene graph from that!  See https://github.com/art-framework-suite/canvas/blob/develop/canvas/Persistency/Common/Ptr.h and https://github.com/art-framework-suite/canvas/blob/develop/canvas/Persistency/Common/Assns.h
- Display context about selected objects.  We could query the ART job like it's a database!  The backend just has to send back HTML.  Some awesome things I want to do:
  - Show ART provenance information for any data product.  Do this on double click?  Or perhaps double right click?
  - Show histograms of the SSDs when their volumes are selected.  Do this on mouse-over?
  - Talk about what magnetic field, alignment constants, and beam conditions are loaded.  Selecting geometry objects is an opportunity to make "ergonomic" controls!
- Advanced extensibility: ART Tools that can send arbitrary cubes that frontend knows how to render.

## Helpful links
- GUI example: https://github.com/georgealways/lil-gui
- three.js manual: https://threejs.org/manual/#en/fundamentals
- Wavefront file format that I'm using TGeoShapes: https://en.wikipedia.org/wiki/Wavefront_.obj_file
