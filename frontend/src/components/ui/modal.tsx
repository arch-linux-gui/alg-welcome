import React from "react";
import "../../globals.css";

interface ModalProps {
  title: string;
  message: string;
  onClose: () => void;
  isDarkMode: boolean;
}

const Modal: React.FC<ModalProps> = ({
  title,
  message,
  onClose,
  isDarkMode,
}) => {
  const modalClasses = isDarkMode
    ? "bg-gray-800 text-white"
    : "bg-white text-black";
  const buttonClasses = isDarkMode
    ? "bg-blue-500 text-white"
    : "bg-blue-500 text-black";

  return (
    <div className="fixed inset-0 flex items-center justify-center bg-black bg-opacity-50 z-50">
      <div className={`p-4 rounded-xl shadow-lg w-70 ${modalClasses}`}>
        <h2
          className={`text-2xl mb-4 text-center font-bold ${
            title === "Success" ? "text-green-600" : "text-red-600"
          }`}
        >
          {title}
        </h2>
        <p>{message}</p>
        <div className="flex justify-center mt-4">
          <button
            onClick={onClose}
            className={`px-4 py-2 rounded button ${buttonClasses}`}
          >
            Close
          </button>
        </div>
      </div>
    </div>
  );
};

export default Modal;
