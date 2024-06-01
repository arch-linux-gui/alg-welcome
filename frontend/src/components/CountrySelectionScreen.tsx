import React, { useState, useEffect } from "react";
import { MirrorList } from "../../wailsjs/go/main/App";
import { EventsOn } from "../../wailsjs/runtime";
import { useLogsContext } from "@/lib/LogsContext";

const CountrySelectionScreen: React.FC<{ isDarkMode: boolean }> = ({
  isDarkMode,
}) => {
  const [selectedCountries, setSelectedCountries] = useState<string[]>([]);
  const [includeHttps, setIncludeHttps] = useState<boolean>(true);
  const [includeHttp, setIncludeHttp] = useState<boolean>(false);
  // const [loading, setLoading] = useState<boolean>(false);
  const { addLog, clearLogs, loading, setLoading } = useLogsContext();

  useEffect(() => {
    const logEventHandler = (message: string) => {
      addLog((prevLogs) => [...prevLogs, message]);
    };

    const unregisterEvent = EventsOn("log", logEventHandler);

    return () => {
      unregisterEvent();
    };
  }, []);

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

  const handleUpdateMirrors = async () => {
    const protocol = `${includeHttps ? "https" : ""}${
      includeHttp ? ",http" : ""
    }`;
    const command = `pkexec reflector --country "${selectedCountries.join(
      ","
    )}" --protocol ${protocol} --latest 20 --sort rate --save /etc/pacman.d/mirrorlist`;

    setLoading(true);
    try {
      await MirrorList(command);
    } catch (error) {
      console.error("Error updating mirrors:", error);
    } finally {
      setLoading(false);
      setSelectedCountries([]);
      setIncludeHttps(true);
      setIncludeHttp(false);
      clearLogs();
    }
  };

  return (
    <div className="p-10 pt-17 w-full h-full flex flex-col">
      <h1 className="text-4xl mt-4 mb-4 text-center font-bold">
        Update MirrorList
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
          <div className="flex items-center">
            <button
              onClick={handleUpdateMirrors}
              className={
                !selectedCountries.length
                  ? `w-[90%] bg-[#6a45d1] text-white px-4 py-2 rounded opacity-50 cursor-not-allowed button`
                  : `w-[90%] bg-[#6a45d1] text-white px-4 py-2 rounded hover:bg-[#7c53ed] button`
              }
              disabled={!selectedCountries.length || loading}
            >
              {loading ? "Updating..." : "Update"}
            </button>
            <span className="ml-2 p-2"></span>
          </div>
        </div>
      </div>
    </div>
  );
};

export default CountrySelectionScreen;
