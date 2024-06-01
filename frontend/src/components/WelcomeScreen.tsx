import React, { useEffect, useState } from "react";
import next from "../assets/next.png";
import about from "../assets/about.png";
import appicon from "../assets/appicon.png";
import {
  UpdateSystem,
  ScreenResolution,
  IsLiveISO,
  RunCalamaresIfLiveISO,
  URL,
} from "../../wailsjs/go/main/App";
import "../globals.css";
import Modal from "./ui/modal";

interface ScreenProps {
  goToScreen: (index: number) => void;
  goBack?: () => void;
  currentScreenIndex?: number;
  isDarkMode: boolean;
}

const WelcomeScreen: React.FC<ScreenProps> = ({ goToScreen, isDarkMode }) => {
  const [modalVisible, setModalVisible] = useState<boolean>(false);
  const [modalTitle, setModalTitle] = useState<string>("");
  const [modalMessage, setModalMessage] = useState<string>("");
  const [loading, setLoading] = useState<boolean>(false);
  const [isInstalled, setIsInstalled] = useState<boolean>(false);
  const [isActiveScreenRes, setIsActiveScreenRes] = useState<boolean>(false);
  const [isActiveInstall, setIsActiveInstall] = useState<boolean>(false);

  useEffect(() => {
    const checkInstallation = async () => {
      const isLive = await IsLiveISO();
      setIsInstalled(isLive);
      console.log(isLive);
    };
    checkInstallation();
  }, []);

  const handleInstallALG = async () => {
    try {
      setIsActiveInstall(true);
      await RunCalamaresIfLiveISO();
    } finally {
      setIsActiveInstall(false);
    }
  };

  const handleUpdateSystem = async () => {
    setLoading(true);
    try {
      await UpdateSystem();
      setModalTitle("Success");
      setModalMessage("System updated successfully.");
    } catch (error) {
      setModalTitle("Error");
      setModalMessage("Failed to update the system.");
    } finally {
      setLoading(false);
      setModalVisible(true);
    }
  };

  const handleScreenResolution = async () => {
    try {
      setIsActiveScreenRes(true);
      await ScreenResolution();
    } catch (error) {
      setModalTitle("Error");
      setModalMessage("Failed to update the system.");
    } finally {
      setIsActiveScreenRes(false);
    }
  };

  return (
    <div className="flex flex-col items-center justify-center h-full space-y-6">
      <img src={appicon} width={150} height={150} alt="App Icon" />
      <h1 className="text-4xl font-bold">Welcome to ALG</h1>
      <div className="grid grid-cols-3 gap-4">
        {isInstalled ? (
          <button
            onClick={handleInstallALG}
            className={
              isActiveInstall
                ? `bg-[#6a45d1] text-white px-4 py-2 rounded opacity-50 cursor-not-allowed button`
                : `bg-[#6a45d1] text-white px-4 py-2 rounded hover:bg-[#7c53ed] button`
            }
            disabled={isActiveInstall}
          >
            Install ALG
          </button>
        ) : (
          <button
            onClick={handleUpdateSystem}
            className={
              loading
                ? `bg-[#6a45d1] text-white px-4 py-2 rounded opacity-50 cursor-not-allowed button`
                : `bg-[#6a45d1] text-white px-4 py-2 rounded hover:bg-[#7c53ed] button`
            }
            disabled={loading}
          >
            <span>Update System</span>
          </button>
        )}
        <button onClick={() => goToScreen(1)} className="button">
          Update Mirrorlist
          <img src={next} alt="Next Icon" className="w-5 h-5" />
        </button>
        <button
          onClick={handleScreenResolution}
          className={
            isActiveScreenRes
              ? `bg-[#6a45d1] text-white px-4 py-2 rounded opacity-50 cursor-not-allowed button`
              : `bg-[#6a45d1] text-white px-4 py-2 rounded hover:bg-[#7c53ed] button`
          }
          disabled={isActiveScreenRes}
        >
          <span>Screen Resolution</span>
        </button>
        <button
          onClick={async () => {
            await URL("https://discord.gg/NgAFEw9Tkf");
          }}
          className="button"
        >
          <span>Discord Server</span>
          <img src={next} alt="Icon" className="w-5 h-5 ml-2" />
        </button>
        <button
          onClick={async () => {
            await URL("https://github.com/arch-linux-gui");
          }}
          className="button"
        >
          <span>GitHub</span>
          <img src={next} alt="Icon" className="w-5 h-5 ml-2" />
        </button>
        <button
          onClick={async () => {
            await URL(
              "https://arch-linux-gui.github.io/web/tutorials/index.html"
            );
          }}
          className="button"
        >
          <span>Tutorial</span>
          <img src={next} alt="Icon" className="w-5 h-5 ml-2" />
        </button>
      </div>
      <button
        onClick={() => goToScreen(2)}
        className="flex items-center justify-center h-12 w-50 about-us"
      >
        <img src={about} alt="About Icon" className="w-6 h-6 mr-2" />
        About Us
      </button>
      {modalVisible && (
        <Modal
          title={modalTitle}
          message={modalMessage}
          onClose={() => setModalVisible(false)}
          isDarkMode={isDarkMode}
        />
      )}
    </div>
  );
};

export default WelcomeScreen;
