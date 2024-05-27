import React, { useState } from "react";
import { cn } from "@/lib/utils";
import { ToggleTheme } from "../wailsjs/go/main/App";
import appicon from "./assets/appicon.png";
import moon from "./assets/moon.png";
import sun from "./assets/sun.png";
import back from "./assets/back.png";
import next from "./assets/next.png";
import about from "./assets/about.png";

interface ScreenProps {
  goToScreen: (index: number) => void;
  goBack?: () => void;
  currentScreenIndex?: number;
}

const App: React.FC = () => {
  const [currentScreen, setCurrentScreen] = useState<number>(0);
  const [isDarkMode, setIsDarkMode] = useState<boolean>(true);
  const [isNavigated, setIsNavigated] = useState<boolean>(false);

  const toggleTheme = () => {
    setIsDarkMode(!isDarkMode);
    ToggleTheme(!isDarkMode);
  };

  const goToScreen = (index: number) => {
    setCurrentScreen(index);
    setIsNavigated(true);
  };

  const goBack = () => {
    setCurrentScreen(0);
    setIsNavigated(false);
  };

  const screens: JSX.Element[] = [
    <WelcomeScreen goToScreen={goToScreen} />,
    <InfoScreen />,
    <AdditionalScreen />,
    <AdditionalScreen />,
    <AdditionalScreen />,
    <AdditionalScreen />,
    <AdditionalScreen />,
    <AboutUs />,
  ];

  return (
    <div
      className={cn(
        `${
          isDarkMode ? "bg-gray-900 text-white" : "bg-white text-black"
        } w-[600px] h-[500px]`,
        { "border border-gray-300": !isDarkMode },
        "select-none"
      )}
    >
      <div className="h-full">
        {React.cloneElement(screens[currentScreen], {
          goToScreen,
          goBack,
          currentScreenIndex: currentScreen,
        })}
      </div>
      {isNavigated && (
        <div className="absolute top-4 left-4">
          <button
            onClick={goBack}
            className="px-2 py-2 pr-2 w-[5rem] bg-blue-600 text-white font-bold flex item-center rounded-2xl hover:bg-blue-700"
          >
            <img src={back} alt="Icon" className="w-5 h-5 mt-[2px]" />
            Back
          </button>
        </div>
      )}
      {currentScreen === 0 && (
        <div className="absolute bottom-4 right-4">
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
                  } inline-block w-5 mt-1 ml-1 mr-1 h-5 bg-white rounded-full shadow-md transform transition-transform duration-300`
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
      )}
    </div>
  );
};

const WelcomeScreen: React.FC<ScreenProps> = ({ goToScreen }) => (
  <div className="flex flex-col items-center justify-center h-full space-y-4">
    <img src={appicon} width={120} height={120} />
    <h1 className="text-2xl font-bold">Welcome to ALG</h1>
    <div className="grid grid-cols-3 gap-x-16 gap-y-4">
      <button
        onClick={() => goToScreen(1)}
        className="px-4 py-2 bg-blue-600 text-white rounded-lg flex items-center hover:bg-blue-700"
      >
        Screen 1
        <img src={next} alt="Icon" className="w-5 h-5 ml-2" />
      </button>
      <button
        onClick={() => goToScreen(1)}
        className="px-4 py-2 bg-blue-600 text-white rounded-lg flex items-center hover:bg-blue-700"
      >
        Screen 2
        <img src={next} alt="Icon" className="w-5 h-5 ml-2" />
      </button>
      <button
        onClick={() => goToScreen(3)}
        className="px-4 py-2 bg-blue-600 text-white rounded-lg flex items-center hover:bg-blue-700"
      >
        Screen 3
        <img src={next} alt="Icon" className="w-5 h-5 ml-2" />
      </button>
      <button
        onClick={() => goToScreen(4)}
        className="px-4 py-2 bg-blue-600 text-white rounded-lg flex items-center hover:bg-blue-700"
      >
        Screen 4
        <img src={next} alt="Icon" className="w-5 h-5 ml-2" />
      </button>
      <button
        onClick={() => goToScreen(5)}
        className="px-4 py-2 bg-blue-600 text-white rounded-lg flex items-center hover:bg-blue-700"
      >
        Screen 5
        <img src={next} alt="Icon" className="w-5 h-5 ml-2" />
      </button>
      <button
        onClick={() => goToScreen(6)}
        className="px-4 py-2 bg-blue-600 text-white rounded-lg flex items-center hover:bg-blue-700"
      >
        Screen 6
        <img src={next} alt="Icon" className="w-5 h-5 ml-2" />
      </button>
    </div>
    <button
      onClick={() => goToScreen(7)}
      className="px-4 py-2 mt-10 bg-blue-600 text-white rounded-lg flex items-center hover:bg-blue-700"
    >
      <img src={about} alt="Icon" className="w-5 h-5 mr-2" />
      About Us
    </button>
  </div>
);

const InfoScreen: React.FC = () => (
  <div className="flex items-center justify-center h-full">
    <p className="text-xl">Here is some useful information.</p>
  </div>
);

const AdditionalScreen: React.FC = () => (
  <div className="flex items-center justify-center h-full">
    <p className="text-xl">This is an additional screen.</p>
  </div>
);

const AboutUs: React.FC = () => (
  <div className="p-8 pt-20 rounded-lg shadow-md max-w-3xl mx-auto ">
    <h1 className="text-4xl font-bold text-center mb-6">
      About Us
    </h1>
    <p className="text-lg leading-relaxed mb-4 text-center">
      Welcome to <span className="font-bold text-blue-600">Arka Linux GUI</span>
      , your gateway to an efficient and powerful Arch-based Linux experience.
      Our mission is to provide a user-friendly and robust environment that
      leverages the strengths of Arch Linux while offering a polished graphical
      interface.
    </p>
    <p className="text-lg leading-relaxed mb-4 text-center">  
      At <span className="font-bold text-blue-600">Arka Linux GUI</span>, we
      believe in the power of simplicity and customization. Whether you are a
      seasoned Linux user or a newcomer, our distribution aims to deliver an
      experience that is both accessible and deeply customizable to suit your
      needs.
    </p>
  </div>
);

export default App;
