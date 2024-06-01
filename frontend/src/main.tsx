import React from "react";
import { createRoot } from "react-dom/client";
import "./globals.css";
import App from "./App";
import { LogsProvider } from "./lib/LogsContext";

const container = document.getElementById("root");

const root = createRoot(container!);

root.render(
  <React.StrictMode>
    <LogsProvider>
      <App />
    </LogsProvider>
  </React.StrictMode>
);
