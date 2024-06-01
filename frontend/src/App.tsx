import React, { useEffect, useState } from "react";
import { cn } from "@/lib/utils";
import {
  ToggleTheme,
  CurrentTheme,
  CheckFileExists,
  ToggleAutostart,
} from "../wailsjs/go/main/App";
import moon from "./assets/moon.png";
import sun from "./assets/sun.png";
import back from "./assets/back.png";
import WelcomeScreen from "./components/WelcomeScreen";
import AboutUs from "./components/AboutUs";
import CountrySelectionScreen from "./components/CountrySelectionScreen";
import logDark from "./assets/logDark.png";
import logLight from "./assets/logLight.png";
import Logger from "./components/Logger";

const App: React.FC = () => {
  const [currentScreen, setCurrentScreen] = useState<number>(0);
  const [isDarkMode, setIsDarkMode] = useState<boolean>(true);
  const [isNavigated, setIsNavigated] = useState<boolean>(false);
  const [isAutoStart, setIsAutoStart] = useState<boolean>(false); // New state for auto start
  const [showLogger, setShowLogger] = useState<boolean>(false); // State for logger screen

  useEffect(() => {
    const fetchTheme = async () => {
      try {
        const currTheme = await CurrentTheme();
        const fileExist = await CheckFileExists();
        setIsAutoStart(fileExist);
        setIsDarkMode(
          currTheme === "Adwaita-dark" ||
            currTheme === "prefer-dark" ||
            currTheme === "org.kde.breezedark.desktop"
        );
      } catch (error) {
        console.error("Failed to fetch current theme:", error);
      }
    };

    fetchTheme();
  }, []);

  const toggleTheme = () => {
    setIsDarkMode(!isDarkMode);
    ToggleTheme(!isDarkMode);
  };

  const toggleAutoStart = async () => {
    const newAutoStartState = !isAutoStart;
    try {
      await ToggleAutostart(newAutoStartState);
      const fileExists = await CheckFileExists();
      setIsAutoStart(fileExists);
    } catch (error) {
      console.error("Failed to toggle autostart:", error);
    }
  };

  const goToScreen = (index: number) => {
    setCurrentScreen(index);
    setIsNavigated(true);
  };

  const goBack = () => {
    setCurrentScreen(0);
    setIsNavigated(false);
    setShowLogger(false);
  };

  const screens: JSX.Element[] = [
    <WelcomeScreen goToScreen={goToScreen} isDarkMode={isDarkMode} />,
    <CountrySelectionScreen isDarkMode={isDarkMode} />,
    <AboutUs />,
  ];

  return (
    <div
      className={cn(
        `${
          isDarkMode ? "bg-gray-900 text-white" : "bg-white text-black"
        } w-[600px] h-[550px]`,
        { "border border-gray-300": !isDarkMode },
        "select-none"
      )}
    >
      <div className="h-full relative">
        {!showLogger ? (
          <>
            {React.cloneElement(screens[currentScreen], {
              goToScreen,
              goBack,
              currentScreenIndex: currentScreen,
            })}
            {isNavigated && (currentScreen === 1 || currentScreen === 2) && (
              <div className="absolute top-4 left-4">
                <button
                  onClick={goBack}
                  className="px-2 py-2 pr-2 w-[5rem] bg-[#6a45d1] text-white font-bold flex item-center rounded-2xl hover:bg-[#7c53ed]"
                >
                  <img src={back} alt="Icon" className="w-5 h-5 mt-[2px]" />
                  Back
                </button>
              </div>
            )}
          </>
        ) : (
          <div className="logger-screen flex flex-col justify-center items-center">
            <h1 className="text-4xl mt-4 text-center font-bold">Logs</h1>
            <Logger isDarkMode={isDarkMode} />
          </div>
        )}
      </div>
      {currentScreen === 1 && (
        <button
          onClick={() => setShowLogger(!showLogger)}
          className={cn(
            `${isDarkMode ? "bg-gray-500" : "bg-gray-300"}
              absolute bottom-1 right-0 mt-7 mr-5 mb-8 p-1 rounded-full`
          )}
        >
          <img
            src={isDarkMode ? logDark : logLight}
            alt="Toggle Logger"
            className="w-6 h-6 mt-1 ml-1 z-50"
          />
        </button>
      )}
      {currentScreen === 0 && !showLogger && (
        <>
          <div className="absolute bottom-4 right-7">
            <div className="flex items-center space-x-2">
              <span className="font-bold">Light</span>
              <button
                onClick={toggleTheme}
                className={cn(
                  `${
                    isDarkMode ? "bg-[#6a45d1]" : "bg-gray-400"
                  } w-11 h-7 rounded-full relative`
                )}
              >
                <span
                  className={cn(
                    `${
                      isDarkMode ? "translate-x-2" : "-translate-x-2"
                    } inline-block w-5 mt-1 ml-1 h-5 bg-white rounded-full shadow-md transform transition-transform duration-300`
                  )}
                >
                  {isDarkMode ? (
                    <img src={moon} alt="Moon" className="w-5 h-5" />
                  ) : (
                    <img src={sun} alt="Sun" className="w-5 h-5" />
                  )}
                </span>
              </button>
              <span className="font-bold">Dark</span>
            </div>
          </div>
          <div className="absolute bottom-4 left-4">
            <div className="flex flex-col items-center">
              <span className="font-bold mb-1">Auto Start</span>
              <div className="flex items-center space-x-2">
                <span className="font-bold">Off</span>
                <button
                  onClick={toggleAutoStart}
                  className={cn(
                    `${
                      isAutoStart ? "bg-[#6a45d1]" : "bg-gray-400"
                    } w-11 h-7 rounded-full relative`
                  )}
                >
                  <span
                    className={cn(
                      `${
                        isAutoStart ? "translate-x-2" : "-translate-x-2"
                      } inline-block w-5 mt-1 ml-1 mr-1 h-5 bg-white rounded-full shadow-md transform transition-transform duration-300`
                    )}
                  />
                </button>
                <span className="font-bold">On</span>
              </div>
            </div>
          </div>
        </>
      )}
    </div>
  );
};

export default App;
