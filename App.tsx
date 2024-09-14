import React, { useState } from 'react';
import { Button } from "@/components/ui/button"
import { Input } from "@/components/ui/input"
import { Label } from "@/components/ui/label"
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card"
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select"
import { Slider } from "@/components/ui/slider"

interface Bot {
  id: string;
  type: string;
  tradingPair: string;
  leverage: number;
  apiKey: string;
  apiSecret: string;
}

const saveBotToAPI = async (bot: Bot): Promise<Bot> => {
  try {
    const response = await fetch('http://localhost:9080/addbot', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(bot),
    });

    if (!response.ok) {
      throw new Error('Failed to save bot');
    }

    const savedBot = await response.json();
    return savedBot;
  } catch (error) {
    console.error(error);
    throw error;
  }
};

export default function TradingBotDashboard() {
  const [bots, setBots] = useState<Bot[]>([]);
  const [newBot, setNewBot] = useState<Bot>({
    id: '',
    type: 'spot',
    tradingPair: '',
    leverage: 1,
    apiKey: '',
    apiSecret: '',
  });
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const handleInputChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const { name, value } = e.target;
    setNewBot(prev => ({ ...prev, [name]: value }));
  };

  const handleTypeChange = (value: string) => {
    setNewBot(prev => ({ ...prev, type: value }));
  };

  const handleLeverageChange = (value: number[]) => {
    setNewBot(prev => ({ ...prev, leverage: value[0] }));
  };

  const handleLeverageInputChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const value = parseInt(e.target.value);
    if (!isNaN(value) && value >= 1 && value <= 100) {
      setNewBot(prev => ({ ...prev, leverage: value }));
    }
  };

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setIsLoading(true);
    setError(null);

    try {
      const savedBot = await saveBotToAPI(newBot);
      setBots(prev => [...prev, savedBot]);
      setNewBot({
        id: '',
        type: 'spot',
        tradingPair: '',
        leverage: 1,
        apiKey: '',
        apiSecret: '',
      });
    } catch (err) {
      setError('Failed to save bot. Please try again.');
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <div className="min-h-screen bg-gray-900 text-gray-100 p-4">
      <div className="container mx-auto">
        <h1 className="text-3xl font-bold mb-6 text-gray-100">Trading Bot Dashboard</h1>
        
        <Card className="mb-8 bg-gray-800 border-gray-700">
          <CardHeader>
            <CardTitle className="text-gray-100">Add New Bot</CardTitle>
            <CardDescription className="text-gray-400">Fill in the details to add a new trading bot.</CardDescription>
          </CardHeader>
          <CardContent>
            <form onSubmit={handleSubmit} className="space-y-4">
              <div>
                <Label htmlFor="id" className="text-gray-200">Bot ID</Label>
                <Input
                  id="id"
                  name="id"
                  value={newBot.id}
                  onChange={handleInputChange}
                  required
                  className="bg-gray-700 text-gray-100 border-gray-600 focus:border-blue-500"
                />
              </div>
              <div>
                <Label htmlFor="type" className="text-gray-200">Bot Type</Label>
                <Select onValueChange={handleTypeChange} value={newBot.type}>
                  <SelectTrigger className="w-full bg-gray-700 text-gray-100 border-gray-600">
                    <SelectValue placeholder="Select bot type" />
                  </SelectTrigger>
                  <SelectContent className="bg-gray-700 text-gray-100 border-gray-600">
                    <SelectItem value="spot">Spot</SelectItem>
                    <SelectItem value="futures">Futures</SelectItem>
                  </SelectContent>
                </Select>
              </div>
              <div>
                <Label htmlFor="tradingPair" className="text-gray-200">Trading Pair</Label>
                <Input
                  id="tradingPair"
                  name="tradingPair"
                  value={newBot.tradingPair}
                  onChange={handleInputChange}
                  required
                  className="bg-gray-700 text-gray-100 border-gray-600 focus:border-blue-500"
                />
              </div>
              <div>
                <Label htmlFor="leverage" className="text-gray-200">Leverage</Label>
                <div className="flex items-center space-x-4">
                  <Slider
                    id="leverageSlider"
                    min={1}
                    max={100}
                    step={1}
                    value={[newBot.leverage]}
                    onValueChange={handleLeverageChange}
                    className="flex-grow"
                  />
                  <Input
                    id="leverage"
                    name="leverage"
                    type="number"
                    min={1}
                    max={100}
                    value={newBot.leverage}
                    onChange={handleLeverageInputChange}
                    className="w-20 bg-gray-700 text-gray-100 border-gray-600 focus:border-blue-500"
                  />
                </div>
              </div>
              <div>
                <Label htmlFor="apiKey" className="text-gray-200">API Key</Label>
                <Input
                  id="apiKey"
                  name="apiKey"
                  value={newBot.apiKey}
                  onChange={handleInputChange}
                  required
                  className="bg-gray-700 text-gray-100 border-gray-600 focus:border-blue-500"
                />
              </div>
              <div>
                <Label htmlFor="apiSecret" className="text-gray-200">API Secret</Label>
                <Input
                  id="apiSecret"
                  name="apiSecret"
                  value={newBot.apiSecret}
                  onChange={handleInputChange}
                  required
                  className="bg-gray-700 text-gray-100 border-gray-600 focus:border-blue-500"
                />
              </div>
              <Button type="submit" disabled={isLoading} className="bg-blue-600 hover:bg-blue-700 text-white">
                {isLoading ? 'Saving...' : 'Save Bot'}
              </Button>
            </form>
          </CardContent>
        </Card>

        {error && <p className="text-red-400 mb-4">{error}</p>}

        <Card className="bg-gray-800 border-gray-700">
          <CardHeader>
            <CardTitle className="text-gray-100">Saved Bots</CardTitle>
            <CardDescription className="text-gray-400">List of all your saved trading bots.</CardDescription>
          </CardHeader>
          <CardContent>
            {bots.length === 0 ? (
              <p className="text-gray-400">No bots saved yet.</p>
            ) : (
              <ul className="space-y-4">
                {bots.map(bot => (
                  <li key={bot.id} className="border border-gray-700 p-4 rounded-md bg-gray-750">
                    <p className="text-gray-300">ID: {bot.id}</p>
                    <h3 className="font-bold text-gray-100">{bot.type}</h3>
                    <p className="text-gray-300">Trading Pair: {bot.tradingPair}</p>
                    <p className="text-gray-300">Leverage: {bot.leverage}</p>
                    <p className="text-gray-300">API Key: {bot.apiKey}</p>
                  </li>
                ))}
              </ul>
            )}
          </CardContent>
        </Card>
      </div>
    </div>
  );
}
