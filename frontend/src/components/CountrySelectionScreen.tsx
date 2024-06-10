import React, { useState, useEffect } from "react";
import { MirrorList } from "../../wailsjs/go/main/App";
import { EventsOn } from "../../wailsjs/runtime";
import { useLogsContext } from "@/lib/LogsContext";

const CountrySelectionScreen: React.FC<{
  isDarkMode: boolean;
  setShowLogger: (log: boolean) => void;
}> = ({ isDarkMode, setShowLogger }) => {
  const [selectedCountries, setSelectedCountries] = useState<string[]>([]);
  const [includeHttps, setIncludeHttps] = useState<boolean>(true);
  const [includeHttp, setIncludeHttp] = useState<boolean>(false);
  const [sortBy, setSortBy] = useState<string>("age");
  const [maxMirrors, setMaxMirrors] = useState<number>(20);
  const [timeout, setTimeout] = useState<number>(30);
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
    "Norway",
    "Brazil",
    "India",
    "Japan",
    "Australia",
    "Sweden",
    "China",
    "France",
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
    )}" --protocol ${protocol} --latest ${maxMirrors} --sort ${sortBy} --download-timeout ${timeout} --save /etc/pacman.d/mirrorlist`;

    setLoading(true);

    try {
      setShowLogger(true);
      await MirrorList(command);
    } catch (error) {
      console.error("Error updating mirrors:", error);
    } finally {
      setLoading(false);
      setShowLogger(false);
      setSelectedCountries([]);
      setIncludeHttps(true);
      setIncludeHttp(false);
      setSortBy("rate");
      setMaxMirrors(20);
      setTimeout(10);
      clearLogs();
    }
  };

  return (
    <div
      className={
        isDarkMode
          ? "p-4 w-[600px] bg-[#090E0E] h-[500px] flex flex-col"
          : "p-4 w-[600px] h-[500px] flex flex-col"
      }
    >
      <h1 className="text-2xl mt-2 mb-4 text-center font-bold">
        Update MirrorList
      </h1>
      <div className="flex-1 flex flex-col space-y-4">
        <div className="flex-1">
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
        <div className="flex flex-col space-y-4">
          <div className="flex space-x-4">
            <div className="flex-1">
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
              <label className="block mb-2">
                <input
                  type="checkbox"
                  checked={includeHttp}
                  onChange={() => setIncludeHttp(!includeHttp)}
                  className="mr-2"
                />
                HTTP
              </label>
            </div>
            <div className="flex-1">
              <h2 className="text-xl mb-2">Sort By</h2>
              <select
                value={sortBy}
                onChange={(e) => setSortBy(e.target.value)}
                className="p-1 border border-gray-300 rounded-md shadow-sm focus:outline-none focus:ring-2 focus:ring-indigo-500 focus:border-indigo-500 bg-white text-gray-700"
              >
                <option value="rate">Rate</option>
                <option value="age">Age</option>
                <option value="country">Country</option>
                <option value="score">Score</option>
                <option value="delay">Delay</option>
              </select>
            </div>
          </div>
          <div className="flex space-x-4">
            <div className="flex-1">
              <h2 className="text-lg mb-2">Max Numbers of Fresh Mirrors</h2>
              <div className="flex items-center space-x-2">
                <button
                  onClick={() => setMaxMirrors((prev) => Math.max(prev - 1, 1))}
                  className="px-2 py-1 border rounded-3xl"
                >
                  -
                </button>
                <input
                  type="number"
                  value={maxMirrors}
                  onChange={(e) => setTimeout(parseInt(e.target.value))}
                  className={
                    isDarkMode
                      ? "w-12 text-center border-none bg-gray-800"
                      : "w-12 text-center border-none bg-gray-200"
                  }
                  min="1"
                />
                <button
                  onClick={() => setMaxMirrors((prev) => prev + 1)}
                  className="px-2 py-1 border rounded-3xl"
                >
                  +
                </button>
              </div>
            </div>
            <div className="flex-1">
              <h2 className="text-lg mb-2">Download Timeout (seconds)</h2>
              <div className="flex items-center space-x-2">
                <button
                  onClick={() => setTimeout((prev) => Math.max(prev - 1, 1))}
                  className="px-2 py-1 border rounded-3xl"
                >
                  -
                </button>
                <input
                  type="number"
                  value={timeout}
                  onChange={(e) => setTimeout(parseInt(e.target.value))}
                  className={
                    isDarkMode
                      ? "w-12 text-center border-none bg-gray-800"
                      : "w-12 text-center border-none bg-gray-200"
                  }
                  min="1"
                />
                <button
                  onClick={() => setTimeout((prev) => prev + 1)}
                  className="px-2 py-1 border rounded-3xl"
                >
                  +
                </button>
              </div>
            </div>
          </div>
        </div>
        <div className="flex items-center justify-center mt-4">
          <button
            onClick={handleUpdateMirrors}
            className={
              !selectedCountries.length
                ? `w-[90%] ${
                    isDarkMode ? "bg-gray-700" : "bg-gray-300"
                  } mr-10 px-4 py-2 rounded-2xl opacity-50 cursor-not-allowed`
                : `w-[90%] ${
                    isDarkMode
                      ? "bg-gray-700 hover:bg-gray-600"
                      : "bg-gray-300 hover:bg-gray-400"
                  } mr-10 px-4 py-2 rounded-2xl`
            }
            disabled={!selectedCountries.length || loading}
          >
            {loading ? "Updating..." : "Update"}
          </button>
        </div>
      </div>
    </div>
  );
};

export default CountrySelectionScreen;
