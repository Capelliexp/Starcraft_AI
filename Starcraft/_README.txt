Install and run the bot
-----------------------
1. Copy the contents of the WINDOWS folder to your C:/Windows folder.
2. Run Chaoslauncher.exe in the Chaoslauncher folder as Administrator.
To run as Administrator, right-click and select "Run as Administrator".
3. Make sure the path to the Starcraft folder is correctly set in Chaoslauncher.
4. Make sure BWAPI Injector 1.16.1 RELEASE and W-MODE is selected. Click Start.
5. The bot should now be started in Starcraft (skip the movie sequence if it is started).
If everything works correctly you should see messages such as "A Terran SCV [...] has been created".

Writing code for the bot
------------------------
1. Install Visual Studio 2013. The installer for the free express version (wdexpress_full.exe) is
included in the Visual Studio 2013 Installer folder. 
Note: The project itself is for Visual Studio 2017, we only need 2013 to get the correct build tools.
2. Open the ExampleAIModule solution.
3. Compile as RELEASE.
4. The dll file is automatically copied to the correct location. You can run it using Chaoslauncher
as previously described.
5. Locate the onFrame method in ExampleAIModule.cpp. Add the following line of code:
Broodwar->printf("It works");
6. Compile and Run. If everything works the text "It works" should be seen when the bot is started.
