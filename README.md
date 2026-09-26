# JAM
JAM is a video game where you build the road network of a city and manage traffic.
[Jump to quickstart](#quickstart)

<small>Tested on Windows and Linux Ubuntu</small>
![Screenshot of game title screen showing many cars navigating a large network of roads and two buttons, load saved game and new game](assets/images/gameimage3.png)
![Screenshot of game simulation window showing many cars navigating a large network of roads](assets/images/gameimage2.png)

# Quickstart

I strongly recommend you check out the [controls](#controls) section before you play the game!

1) Download the ZIP file for your operating system here: https://github.com/tiydwe/jam/releases. I recommend downloading the static version, as the shared version has known issues.
For example, if you are using windows, you should download `Windows-Static.zip`.

2) Extract all files into a location of your choice.
3) Run `main` depending on what OS you have. 

## My computer is blocking me from running `main`!

Follow these steps to unblock the executable:

### MacOS

1) Attempt to run it.
2) Open the Apple Menu and select System Settings
3) Click Privacy & Security
4) Go to the Security section
5) Click Open Anyway next to the message about the executable you just tried to run
6) Enter your password and click Open to confirm

### Windows

1) Click the <u>More info</u> text.
2) Click Run Anyway

### Linux

1) Open a terminal
2) Navigate to the folder you extracted the files into
3) Run `chmod +x main`

# Features
* Place multiple different types of roads
* Watch cars try to navigate your road network
* See how well you did!
* Save / load games

# Controls
* To pan: right click + drag
* Zoom in/out: scroll
* Cancel road building (if you have already clicked and started building the road): `esc`
* Hover over cars to see their destination


# How it works
Jam is built off of a custom made traffic engine. Internally, roads are represented as individual one-way edges on a graph and intersections as nodes. To create the UI, a seperate set of classes represent the physical aspects of objects. This keeps the responsibilities seperate and allows simulation to be seperate from the layout. Smart pointers are extensivly used throughout the project to clarify ownership and ease memory management.

The game was built off of SFML and portable-file-dialogs without any additional helpers. Every single UI element was created by hand.

# Credits
Thank you so much to [SFML](https://github.com/sfml/sfml) and [portable file dialogs](https://github.com/samhocevar/portable-file-dialogs)! The Montserrat font was used in this project. The license and attribution is [here](/assets/fonts/OFL.txt).