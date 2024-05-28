import React, { useState } from "react";
import { MirrorList } from "../../wailsjs/go/main/App";
import Modal from "./ui/modal";

interface CountrySelectionScreenProps {
  isDarkMode: boolean;
}

const CountrySelectionScreen: React.FC<CountrySelectionScreenProps> = ({
  isDarkMode,
}) => {
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
      setSelectedCountries([]);
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
                  checked={selectedCountries.includes(country)}
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
          isDarkMode={isDarkMode}
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

export default CountrySelectionScreen;
