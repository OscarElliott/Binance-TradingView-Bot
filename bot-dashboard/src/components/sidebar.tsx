import React, { useState } from 'react'
import { 
  HomeIcon, 
  CogIcon, 
  PlusCircleIcon, 
  QueueListIcon, 
  ChartBarIcon,
  Bars3Icon,
  XMarkIcon
} from '@heroicons/react/24/outline'

export default function Sidebar() {
    const [isOpen, setIsOpen] = useState(false);
    const [botsExpanded, setBotsExpanded] = useState(true);
  
    const toggleSidebar = () => setIsOpen(!isOpen);
    const toggleBotsSection = () => setBotsExpanded(!botsExpanded);
  
    return (
      <>
        {/* Mobile menu button */}
        <button
          className="lg:hidden fixed top-4 left-4 z-20 p-2 rounded-md text-gray-400 hover:text-white hover:bg-gray-700 focus:outline-none focus:ring-2 focus:ring-inset focus:ring-white"
          onClick={toggleSidebar}
        >
          {isOpen ? (
            <XMarkIcon className="h-6 w-6" aria-hidden="true" />
          ) : (
            <Bars3Icon className="h-6 w-6" aria-hidden="true" />
          )}
        </button>
  
        {/* Sidebar */}
        <div
          className={`fixed inset-y-0 left-0 z-10 w-64 bg-gray-800 overflow-y-auto transition duration-300 transform ${
            isOpen ? 'translate-x-0' : '-translate-x-full'
          } lg:translate-x-0 lg:static lg:inset-0`}
        >
          <div className="flex items-center justify-center h-16 bg-gray-900">
            <span className="text-white text-2xl font-semibold">
                <img className='logo' src="src/assets/Bi-Bot.png" alt="Logo" />
            </span>
          </div>
          <nav className="mt-5">
            <a href="/" className="flex items-center px-6 py-2 text-gray-100 hover:bg-gray-700">
              <HomeIcon className="h-6 w-6 mr-3" />
              Dashboard
            </a>
            <div>
              <button onClick={toggleBotsSection} className="flex items-center px-6 py-2 w-full text-gray-100 hover:bg-gray-700 strong_overide_button" >
                <QueueListIcon className="h-6 w-6 mr-3" />
                Trading Bots
                <svg
                  className={`ml-auto h-5 w-5 transform ${botsExpanded ? 'rotate-180' : ''}`}
                  xmlns="http://www.w3.org/2000/svg"
                  fill="none"
                  viewBox="0 0 24 24"
                  stroke="currentColor"
                >
                  <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M19 9l-7 7-7-7" />
                </svg>
              </button>
              {botsExpanded && (
                <div className="bg-gray-700 py-2">
                  <a href="/add-bots" className="flex items-center pl-12 py-2 text-sm text-gray-100 hover:bg-gray-600">
                    <PlusCircleIcon className="h-5 w-5 mr-3" />
                    Add Bot
                  </a>
                  <a href="/manage-bots" className="flex items-center pl-12 py-2 text-sm text-gray-100 hover:bg-gray-600">
                    <QueueListIcon className="h-5 w-5 mr-3" />
                    Manage Bots
                  </a>
                  <a href="#" className="flex items-center pl-12 py-2 text-sm text-gray-100 hover:bg-gray-600">
                    <ChartBarIcon className="h-5 w-5 mr-3" />
                    Analytics
                  </a>
                </div>
              )}
            </div>
            <a href="/settings" className="flex items-center px-6 py-2 text-gray-100 hover:bg-gray-700">
              <CogIcon className="h-6 w-6 mr-3" />
              Settings
            </a>
          </nav>
        </div>
      </>
    );
}