import React, { createContext, useContext, useState, ReactNode } from "react";

interface LogsContextType {
  logs: string[];
  addLog: (log: string | ((prevLogs: string[]) => string[])) => void;
  clearLogs: () => void;
  loading: boolean;
  setLoading: (loading: boolean) => void;
  isEmpty: () => boolean;
}

const LogsContext = createContext<LogsContextType>({
  logs: [],
  addLog: () => {},
  clearLogs: () => {},
  loading: false,
  setLoading: () => {},
  isEmpty: () => {
    return false;
  },
});

export const useLogsContext = (): LogsContextType => {
  const context = useContext(LogsContext);
  if (!context) {
    throw new Error("useLogsContext must be used within a LogsProvider");
  }
  return context;
};

interface LogsProviderProps {
  children: ReactNode;
}

export const LogsProvider: React.FC<LogsProviderProps> = ({ children }) => {
  const [loading, setLoading] = useState<boolean>(false);
  const [logs, setLogs] = useState<string[]>([]);

  const addLog = (log: string | ((prevLogs: string[]) => string[])) => {
    setLogs((prevLogs) =>
      typeof log === "string" ? [...prevLogs, log] : log(prevLogs)
    );
  };

  const clearLogs = () => {
    setLogs([]);
  };

  const isEmpty = () => {
    return logs.toString() === "";
  };

  return (
    <LogsContext.Provider
      value={{ logs, addLog, loading, setLoading, clearLogs, isEmpty }}
    >
      {children}
    </LogsContext.Provider>
  );
};
