import React from 'react';
import TradingBotDashboard from './components/bot-dashboard';
import Sidebar from './components/sidebar';

export default function App() {
  return (
    <div className="flex app-body bg-gray-900 text-gray-100">
      <div>
        <Sidebar />
      </div>
      <main className="flex-1  min-h-screen p-4">
        <TradingBotDashboard />
      </main>
    </div>
  );
}