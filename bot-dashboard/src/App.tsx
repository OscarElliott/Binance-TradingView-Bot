import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import TradingBotDashboard from './components/add-bots';
import Sidebar from './components/sidebar';
import ManageBots from './components/manage-bots';
import Settings from './components/settings';
import UserDashboard from './components/dashboard';

export default function App() {
  return (
    <Router>
      <div className="flex app-body bg-gray-900 text-gray-100">
        <Sidebar />
        <main className="flex-1 overflow-y-scroll min-h-screen p-4">
          <Routes>
            <Route path="/" element={<UserDashboard />} />
            <Route path="/add-bots" element={<TradingBotDashboard />} />
            <Route path="/manage-bots" element={<ManageBots />} />
            <Route path="/settings" element={<Settings />} />
            {/* Add more routes for other components */}
          </Routes>
        </main>
      </div>
    </Router>
  );
}
