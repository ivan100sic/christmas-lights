# christmas-lights

![Demo](https://raw.githubusercontent.com/ivan100sic/christmas-lights/master/demo.gif)

Light up your desktop! Just download and run the app. No configuration required.

**How do I know you're not going to hack me?**

Good question - you should always ask it!

Both your browser and your OS will try to prevent you from downloading and running this app. That's perfectly fine.

In that case, `git clone` and look at the source code. You can build the exe yourself using Visual Studio 2019.
Since the file CImg.h is quite big, you can compare it to the CImg release version 267. The file in this project contains a few changes
related to how a Windows window is created by default. The app uses a borderless window.

**How do I quit the app?**

You will see the app running in the taskbar, you can quit it from the taskbar in the same way as any other,
right click and then "Close window".
