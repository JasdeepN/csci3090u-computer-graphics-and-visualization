Environment Setup:
1.		Install Visual Studio Express 2015 for Desktop, or open your own Visual Studio installation
2. 		Try New -> Project -> Visual C++. If the only options are "Install" then go ahead and "Install Visual C++ 2015 Tools for Windows Desktop".
3.		Unzip csci3090-examples.zip into C:\CSCI 3090.  If you use a different directory, you will have to modify project configurations, so it is recommended to use this directory.
4.		Confirm that you have the examples, freeglut, FreeImage, glew and glm folders to the directory created in step 3. 

Running an Example:

1. 		Open the example .sln file (note, NOT the .vcproj file). If prompted to upgrade solution to latest Visual Studio, click "OK".  
2. 		Build and run in Win32/Debug mode.  Note that you should just be able to click the green "play" button to do this.
3. 		If you receive an error about missing MSVCR100D.dll (common on Windows 10), copy this file from C:\CSCI 3090\MSVCR100D.dll into your Visual Studio VC\bin folder (usually C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin for Visual Studio 2015).

New Project Setup:
1.		Launch Visual Studio
2.		File -> New Project
3.		In the New Project dialog box, Templates -> Visual C++ -> Win32 Console Application (again, avoid using spaces in your project name). Set the location of the project to the directory created in step 3 in Environment Setup. Click OK.
4.		In the Win32 Application Wizard, click Next. In Additional options, check Empty Project. Click Finish.
5.		Project -> [Project Name] Properties
6.		Edit Configuration Properties -> VC++ Directories -> Include Directories to add paths to the include directories under both the freeglut and glew directories and path to the glm directory itself, separated by semicolons.
		If you used the default directories, then this would be: C:\CSCI 3090\glm;C:\CSCI 3090\glew\include;C:\CSCI 3090\freeglut\include;%(AdditionalIncludeDirectories)
7.		Edit Configuration Properties -> VC++ Directories -> Library Directories to add path to the lib directory under the freeglut directory and path to the lib\Release\Win32 directory under the glew directory. 
		If you used the default directories, then this would be: C:\CSCI 3090\glew\lib\Release\Win32;C:\CSCI 3090\freeglut\lib\x86\Debug;%(AdditionalLibraryDirectories)
8.		Add freeglut.lib and glew32.lib to Linker -> Input -> Additional Dependencies. Then, click Apply and OK.
9.		Copy freeglut.dll and glew32.dll to the project directory (using File Explorer). Shader files (.vs and .fs) should also be placed in this directory.
10. 		You can add your shader files as sources: right click on Source Files or Header Files folders, click on Add, click on Existing Item, and choose them.
11.		In the Solution Explorer, right click on Source Files or Header Files folders, click on Add, click on New Item to create your .cpp and .h files respectively.  You can start with files from the examples. 
