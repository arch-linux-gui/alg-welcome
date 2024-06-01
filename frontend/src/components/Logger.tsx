import React, { useEffect, useRef } from "react";
import { EventsOn } from "../../wailsjs/runtime/runtime";
import { useLogsContext } from "@/lib/LogsContext";

interface LoggerProps {
  isDarkMode: boolean;
}

const Logger: React.FC<LoggerProps> = ({ isDarkMode }) => {
  const { logs, addLog, isEmpty } = useLogsContext();
  const loggerRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    const logEventHandler = (message: string) => {
      addLog((prevLogs) => [...prevLogs, message]);
    };

    const unregisterEvent = EventsOn("log", logEventHandler);

    return () => {
      unregisterEvent();
    };
  }, []);

  useEffect(() => {
    if (loggerRef.current) {
      loggerRef.current.scrollTop = loggerRef.current.scrollHeight;
    }
  }, [logs]);

  return (
    <div className="flex flex-col items-center justify-center w-[95%] h-full">
      <div
        ref={loggerRef}
        className={`mt-4 h-[400px] w-full rounded-lg overflow-y-auto overflow-x-hidden ${
          isDarkMode ? "bg-gray-600 text-white" : "bg-gray-200 text-black"
        }`}
        style={{ maxHeight: "400px" }}
      >
        <div className="flex flex-col space-y-2 m-5 text-pretty">
          {logs.map((log, index) => (
            <div key={index}>{"◉ " + log}</div>
          ))}
        </div>
      </div>
    </div>
  );
};

export default Logger;
