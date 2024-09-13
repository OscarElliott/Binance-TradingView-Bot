import logo from './logo.svg';
import './App.css';
import React, { useState } from 'react';

function App() {
  const [apiKey, setApiKey] = useState('');
  const [apiSecret, setApiSecret] = useState('');

  const handleSubmit = (e) => {
      e.preventDefault();
      // Save configuration (send it to your backend or save locally)
      console.log({ apiKey, apiSecret });
  };

  return (
      <div>
          <h1>Bot Configuration</h1>
          <form onSubmit={handleSubmit}>
              <label>
                  API Key:
                  <input type="text" value={apiKey} onChange={(e) => setApiKey(e.target.value)} />
              </label>
              <br />
              <label>
                  API Secret:
                  <input type="text" value={apiSecret} onChange={(e) => setApiSecret(e.target.value)} />
              </label>
              <br />
              <button type="submit">Save</button>
          </form>
      </div>
  );
}

export default App;