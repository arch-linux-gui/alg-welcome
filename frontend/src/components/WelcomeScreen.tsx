import React, { useState } from "react";
import next from "../assets/next.png";
import about from "../assets/about.png";
import appicon from "../assets/appicon.png";
import { UpdateSystem } from "../../wailsjs/go/main/App";
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

  const handleUpdateSystem = async () => {
    try {
      await UpdateSystem();
      setModalTitle("Success");
      setModalMessage(`System updated successfully.`);
      setModalVisible(true);
    } catch (error) {
      setModalTitle("Error");
      setModalMessage("Failed to update the system.");
      setModalVisible(true);
    }
  };

  return (
    <div className="flex flex-col items-center justify-center h-full space-y-4">
      <img src={appicon} width={120} height={120} />
      <h1 className="text-2xl font-bold">Welcome to ALG</h1>
      <div className="grid grid-cols-3 gap-4">
        <button onClick={handleUpdateSystem} className="button">
          <span>Update System</span>
        </button>
        <button onClick={() => goToScreen(1)} className="button">
          Update Mirrorlist
          <img src={next} alt="Icon" className="w-5 h-5" />
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
      <button
        onClick={() => goToScreen(7)}
        className="mt-10 flex items-center justify-center h-12 w-50 about-us"
      >
        <img src={about} alt="Icon" className="w-6 h-6 mr-2" />
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
