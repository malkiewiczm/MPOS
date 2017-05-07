# MPOS
## Music player or something

I was tired of having to use my mouse in order to use windows media player to listen to albums, so I made this thing to play music using bass.dll. Also I felt like making something in C.

This repository only exists so I don't lose the source code, I am probably the only person who finds it intuitive to use, but if you want to mess around with it, instructions are below.

### Dependencies
- Win32
- bass.dll (bass.lib to compile with)
( Make sure you change the TARGET in the Makefile to a location that actually exists before you compile !)

### Db File
The program requires a file called `song_lists.txt` to be present in the same directory as the executable. Contrary to the name, it's actually a list of artists and albums as sub-folders, and DOES NOT REQUIRE YOU TO LIST FILENAMES.

The file is meant to be used folders of the artist name, and each of those folders containing a folder for each album.

The basic format is as follows:
```
+Artist name
-Album 1
-Album 2
-Album 3
+Another artist
-An album by this artist
```
`+` Indicates an artist
`-` Indicates an album IN A FOLDER OF THE SAME NAME
`_` Indicates an album title that IS NOT IN A FOLDER NAMED THE SAME
`/` Indicates the path of the album if the above mark is used
Example:
```
+Artist name
_Album 1
/Path\to\directory_of_album_1
-Album2
+Another artist
-An album by this artist
```

### Navigation in the program
The first screen is your current playlist, press SPACE or TAB to move to the next screen, where you can select an artist. Choose an artist by typing the number and pressing TAB or SPACE. Select an album using the same method. On an album you can either select an individual song to add, or stay on this screen to act on the whole album. Press W to add the songs to your playlist. Press R to clear you playlist and set it to the songs you are viewing. Press Q to cancel and go up a screen. When a song is playing, press P to toggle play / pause, or O to kill the current song. On the playlist screen, pressing R will kill the current song and move to the next. ALTERNATIVELY you can use the media keys for play / pause, stop, and skip.

