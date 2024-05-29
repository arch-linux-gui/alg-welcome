import React, { useEffect, useState } from "react";
import next from "../assets/next.png";
import about from "../assets/about.png";
import appicon from "../assets/appicon.png";
import {
  UpdateSystem,
  ScreenResolution,
  IsLiveISO,
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

  useEffect(() => {
    const checkInstallation = async () => {
      const isLive = await IsLiveISO();
      setIsInstalled(isLive);
      console.log(isLive);
    };
    checkInstallation();
  }, []);

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
    setLoading(true);
    try {
      await ScreenResolution();
    } catch (error) {
      setModalTitle("Error");
      setModalMessage("Failed to update the system.");
    } finally {
      setLoading(false);
      setModalVisible(true);
    }
  };

  return (
    <div className="flex flex-col items-center justify-center h-full space-y-6">
      <img src={appicon} width={150} height={150} alt="App Icon" />
      <h1 className="text-4xl font-bold">Welcome to ALG</h1>
      <div className="grid grid-cols-3 gap-4">
        {isInstalled ? (
          <button onClick={() => {}} className="button">
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
        <button onClick={handleScreenResolution} className="button">
          <span>Screen Resolution</span>
        </button>
        <button onClick={() => goToScreen(4)} className="button">
          <span>Discord Server</span>
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
      <button
        onClick={() => goToScreen(7)}
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