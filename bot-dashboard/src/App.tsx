import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import TradingBotDashboard from './components/add-bots';
import Sidebar from './components/sidebar';
import ManageBots from './components/manage-bots'; // Import other pages as needed

export default function App() {
  return (
    <Router>
      <div className="flex app-body bg-gray-900 text-gray-100">
        <Sidebar />
        <main className="flex-1 min-h-screen p-4">
          <Routes>
          <Route path="/" element={<h1>Dashboard</h1>} />
            <Route path="/add-bots" element={<TradingBotDashboard />} />
            <Route path="/manage-bots" element={<ManageBots />} />
            {/* Add more routes for other components */}
          </Routes>
        </main>
      </div>
    </Router>
  );
}
