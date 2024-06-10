import React, { useEffect, useState } from "react";
import { cn } from "@/lib/utils";
import {
  ToggleTheme,
  CurrentTheme,
  CheckFileExists,
  ToggleAutostart,
} from "../wailsjs/go/main/App";
import back from "./assets/back.png";
import backDark from "./assets/back-dark.png";
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
  const [isAutoStart, setIsAutoStart] = useState<boolean>(false);
  const [showLogger, setShowLogger] = useState<boolean>(false);
  const Themes = [
    "Adwaita-dark",
    "prefer-dark",
    "org.kde.breezedark.desktop",
    "com.github.vinceliuice.Qogir-dark",
  ];

  useEffect(() => {
    const fetchTheme = async () => {
      try {
        const currTheme = await CurrentTheme();
        const fileExist = await CheckFileExists();
        setIsAutoStart(fileExist);
        setIsDarkMode(Themes.includes(currTheme));
      } catch (error) {
        console.error("Failed to fetch current theme:", error);
      }
    };

    fetchTheme();
  }, []);

  const toggleTheme = async () => {
    const newThemeState = !isDarkMode;
    setIsDarkMode(newThemeState);
    await ToggleTheme(newThemeState);
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

  const setterLogger = (log: boolean) => {
    setShowLogger(log);
  };

  const screens: JSX.Element[] = [
    <WelcomeScreen
      goToScreen={goToScreen}
      isDarkMode={isDarkMode}
      toggleDarkMode={toggleTheme}
      isAutoStart={isAutoStart}
      toggleAutoStart={toggleAutoStart}
    />,
    <CountrySelectionScreen
      isDarkMode={isDarkMode}
      setShowLogger={setterLogger}
    />,
    <AboutUs />,
  ];

  return (
    <div
      className={cn(
        `${
          isDarkMode ? "bg-[#090E0E] text-white" : "bg-gray-100 text-gray-800"
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
                  className={`flex item-center w-full py-2 px-4 pr-6 rounded-2xl ${
                    isDarkMode
                      ? "bg-gray-700 hover:bg-gray-600"
                      : "bg-gray-300 hover:bg-gray-400"
                  }`}
                >
                  {isDarkMode ? (
                    <img src={back} alt="Icon" className="w-5 h-5 mt-[2px]" />
                  ) : (
                    <img
                      src={backDark}
                      alt="Icon"
                      className="w-5 h-5 mt-[2px]"
                    />
                  )}
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
    </div>
  );
};

export default App;
