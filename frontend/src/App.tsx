import React, { useEffect, useState } from "react";
import { cn } from "@/lib/utils";
import { ToggleTheme, MirrorList, CurrentTheme } from "../wailsjs/go/main/App";
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

interface ModalProps {
  title: string;
  message: string;
  onClose: () => void;
}

const App: React.FC = () => {
  const [currentScreen, setCurrentScreen] = useState<number>(0);
  const [isDarkMode, setIsDarkMode] = useState<boolean>(true);
  const [isNavigated, setIsNavigated] = useState<boolean>(false);

  useEffect(() => {
    const fetchTheme = async () => {
      try {
        const currTheme = await CurrentTheme();
        setIsDarkMode(
          currTheme == "Adwaita-dark" || currTheme == "prefer-dark"
        );
      } catch (error) {
        console.error("Failed to fetch current theme:", error);
      }
    };

    fetchTheme();
  }, []);

  const toggleTheme = () => {
    const newDarkMode = !isDarkMode;
    setIsDarkMode(newDarkMode);
    ToggleTheme(newDarkMode);
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
    <CountrySelectionScreen />,
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
            className="px-2 py-2 pr-2 w-[5rem] bg-[#6a45d1] text-white font-bold flex item-center rounded-2xl hover:bg-[#7c53ed]"
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
    <div className="grid grid-cols-3 gap-4">
      <button onClick={() => goToScreen(2)} className="button">
        <span>Tutorial</span>
        <img src={next} alt="Icon" className="w-5 h-5 ml-2" />
      </button>
      <button onClick={() => goToScreen(1)} className="button">
        Update Mirrorlist
      </button>
      <button onClick={() => goToScreen(3)} className="button">
        <span>Screen 3</span>
        <img src={next} alt="Icon" className="w-5 h-5 ml-2" />
      </button>
      <button onClick={() => goToScreen(4)} className="button">
        <span>Screen 4</span>
        <img src={next} alt="Icon" className="w-5 h-5 ml-2" />
      </button>
      <button onClick={() => goToScreen(5)} className="button">
        <span>Screen 5</span>
        <img src={next} alt="Icon" className="w-5 h-5 ml-2" />
      </button>
      <button onClick={() => goToScreen(6)} className="button">
        <span>FAQ</span>
        <img src={next} alt="Icon" className="w-5 h-5 ml-2" />
      </button>
    </div>
    <button onClick={() => goToScreen(7)} className="button mt-10">
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
    <h1 className="text-4xl font-bold text-center mb-6">About Us</h1>
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

const Modal: React.FC<ModalProps> = ({ title, message, onClose }) => {
  return (
    <div className="fixed inset-0 flex items-center justify-center bg-black bg-opacity-50 z-50">
      <div className="bg-gray-800 p-4 rounded shadow-lg w-70">
        <h2 className="text-xl mb-4 text-white text-center">{title}</h2>
        <p className="text-white">{message}</p>
        <div className="flex justify-center mt-4">
          <button
            onClick={onClose}
            className="bg-blue-500 text-white px-4 py-2 rounded"
          >
            Close
          </button>
        </div>
      </div>
    </div>
  );
};

const CountrySelectionScreen: React.FC = () => {
  const [selectedCountries, setSelectedCountries] = useState<string[]>([]);
  const [includeHttps, setIncludeHttps] = useState<boolean>(true);
  const [includeHttp, setIncludeHttp] = useState<boolean>(false);
  const [modalVisible, setModalVisible] = useState<boolean>(false);
  const [modalTitle, setModalTitle] = useState<string>("");
  const [modalMessage, setModalMessage] = useState<string>("");
  const [loading, setLoading] = useState<boolean>(false);

  const countries = [
    "United States",
    "France",
    "Germany",
    "India",
    "Norway",
    "Australia",
    "Sweden",
    "Canada",
    "Japan",
    "United Kingdom",
  ];

  const handleCountryChange = (country: string) => {
    setSelectedCountries((prev) =>
      prev.includes(country)
        ? prev.filter((c) => c !== country)
        : [...prev, country]
    );
  };

  const handleSubmit = async () => {
    const protocol = `${includeHttps ? "https" : ""}${
      includeHttp ? ",http" : ""
    }`;
    const command = `pkexec reflector --country "${selectedCountries.join(
      ","
    )}" \
      --verbose \
      --sort rate \
      --protocol ${protocol} \
      --latest 20 \
      --save /etc/pacman.d/mirrorlist`;

    setLoading(true);

    try {
      await MirrorList(command);
      setModalTitle("Success");
      setModalMessage("Mirrorlist updated successfully!");
      setModalVisible(true);
      // alert("Success"); // Conflicted between Alert and Custon Modal
    } catch (error) {
      setModalTitle("Error");
      setModalMessage("Failed to update mirrorlist.");
      setModalVisible(true);
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="p-10 pt-17 w-full h-full flex flex-col">
      <h1 className="text-4xl mt-4 mb-4 text-center font-bold">
        Select Arch Mirrors
      </h1>
      <div className="flex-1 flex flex-col lg:flex-row lg:space-x-4">
        <div className="mb-4 lg:mb-0 lg:flex-1">
          <h2 className="text-xl mb-2">Countries</h2>
          <div className="grid grid-cols-2 gap-2">
            {countries.map((country) => (
              <label key={country} className="block">
                <input
                  type="checkbox"
                  value={country}
                  onChange={() => handleCountryChange(country)}
                  className="mr-2 p-2"
                />
                {country}
              </label>
            ))}
          </div>
        </div>
        <div className="lg:flex-1 flex flex-col justify-between">
          <div>
            <h2 className="text-xl mb-2">Protocols</h2>
            <label className="block mb-2">
              <input
                type="checkbox"
                checked={includeHttps}
                onChange={() => setIncludeHttps(!includeHttps)}
                className="mr-2"
              />
              HTTPS
            </label>
            <label className="block mb-4">
              <input
                type="checkbox"
                checked={includeHttp}
                onChange={() => setIncludeHttp(!includeHttp)}
                className="mr-2"
              />
              HTTP
            </label>
          </div>
          <button
            onClick={handleSubmit}
            className={
              !selectedCountries.length
                ? `bg-[#6a45d1] text-white px-4 py-2 rounded opacity-50 cursor-not-allowed button`
                : `bg-[#6a45d1] text-white px-4 py-2 rounded hover:bg-[#7c53ed] button`
            }
            disabled={!selectedCountries.length || loading}
          >
            Update Mirrorlist
          </button>
        </div>
      </div>
      {modalVisible && (
        <Modal
          title={modalTitle}
          message={modalMessage}
          onClose={() => setModalVisible(false)}
        />
      )}
      {loading && (
        <div className="fixed top-0 left-0 right-0 bottom-0 flex justify-center items-center bg-gray-900 bg-opacity-50 z-50">
          <div className="w-12 h-12 border-4 border-gray-200 rounded-full animate-spin"></div>
        </div>
      )}
    </div>
  );
};
export default App;
