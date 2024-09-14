import React, { useEffect, useState } from 'react';
import { Button } from "@/components/ui/button"
import { Input } from "@/components/ui/input"
import { Label } from "@/components/ui/label"
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card"
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select"
import { CustomSlider } from "@/components/ui/custom-slider"

interface Bot {
  id: string;
  type: string;
  tradingPair: string;
  leverage: number;
}

const fetchBotsFromAPI = async (): Promise<Bot[]> => {
  try {
    const response = await fetch('http://localhost:9080/getbots');
    if (!response.ok) {
      throw new Error('Failed to fetch bots');
    }
    const bots = await response.json();
    return bots;
  } catch (error) {
    console.error(error);
    throw error;
  }
};

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

export default function ManageBots() {
  const [bots, setBots] = useState<Bot[]>([]);
  const [selectedBot, setSelectedBot] = useState<Bot | null>(null);
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  useEffect(() => {
    const loadBots = async () => {
      try {
        const fetchedBots = await fetchBotsFromAPI();
        setBots(fetchedBots);
      } catch (err) {
        setError('Failed to load bots.');
      }
    };
    loadBots();
  }, []);

  const handleBotSelect = (bot: Bot) => {
    setSelectedBot(bot);
  };

  const handleInputChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const { name, value } = e.target;
    setSelectedBot(prev => prev ? { ...prev, [name]: value } : null);
  };

  const handleTypeChange = (value: string) => {
    setSelectedBot(prev => prev ? { ...prev, type: value } : null);
  };

  const handleLeverageChange = (value: number[]) => {
    setSelectedBot(prev => prev ? { ...prev, leverage: value[0] } : null);
  };

  const handleLeverageInputChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const value = parseInt(e.target.value);
    if (!isNaN(value) && value >= 1 && value <= 100) {
      setSelectedBot(prev => prev ? { ...prev, leverage: value } : null);
    }
  };

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!selectedBot) return;
    setIsLoading(true);
    setError(null);

    try {
      const savedBot = await saveBotToAPI(selectedBot);
      setBots(prev => prev.map(bot => (bot.id === savedBot.id ? savedBot : bot)));
    } catch (err) {
      setError('Failed to save bot. Please try again.');
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <div className="min-h-screen bg-gray-900 text-gray-100 p-4">
      <div className="container mx-auto">
        <h1 className="text-3xl font-bold mb-6 text-gray-100">Manage Bots</h1>

        <Card className="mb-8 bg-gray-800 border-gray-700">
          <CardHeader>
            <CardTitle className="text-gray-100">Select a Bot</CardTitle>
            <CardDescription className="text-gray-400">Choose a bot to edit.</CardDescription>
          </CardHeader>
          <CardContent>
            {bots.length === 0 ? (
              <p className="text-gray-400">No bots available.</p>
            ) : (
              <ul className="space-y-4">
                {bots.map(bot => (
                  <li key={bot.id} className="border border-gray-700 p-4 rounded-md bg-gray-750">
                    <button
                      onClick={() => handleBotSelect(bot)}
                      className="text-gray-300 hover:underline">
                      {bot.type} - {bot.tradingPair}
                    </button>
                  </li>
                ))}
              </ul>
            )}
          </CardContent>
        </Card>

        {selectedBot && (
          <Card className="mb-8 bg-gray-800 border-gray-700">
            <CardHeader>
              <CardTitle className="text-gray-100">Edit Bot</CardTitle>
              <CardDescription className="text-gray-400">Edit the selected bot's details.</CardDescription>
            </CardHeader>
            <CardContent>
              <form onSubmit={handleSubmit} className="space-y-4">
                <div>
                  <Label htmlFor="id" className="text-gray-200">Bot ID</Label>
                  <Input
                    id="id"
                    name="id"
                    value={selectedBot.id}
                    onChange={handleInputChange}
                    required
                    className="bg-gray-700 text-gray-100 border-gray-600 focus:border-blue-500"
                  />
                </div>
                <div>
                  <Label htmlFor="type" className="text-gray-200">Bot Type</Label>
                  <Select onValueChange={handleTypeChange} value={selectedBot.type}>
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
                  <Label htmlFor="leverage" className="text-gray-200">Leverage</Label>
                  <div className="flex items-center space-x-4">
                    <CustomSlider
                      id="leverageSlider"
                      min={1}
                      max={100}
                      step={1}
                      value={[selectedBot.leverage]}
                      onValueChange={handleLeverageChange}
                      className="flex-grow"
                    />
                    <Input
                      id="leverage"
                      name="leverage"
                      type="number"
                      min={1}
                      max={100}
                      value={selectedBot.leverage}
                      onChange={handleLeverageInputChange}
                      className="w-20 bg-gray-700 text-gray-100 border-gray-600 focus:border-blue-500"
                    />
                  </div>
                </div>
                <div>
                  <Label htmlFor="tradingPair" className="text-gray-200">Trading Pair</Label>
                  <Input
                    id="tradingPair"
                    name="tradingPair"
                    value={selectedBot.tradingPair}
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
        )}

        {error && <p className="text-red-400 mb-4">{error}</p>}
      </div>
    </div>
  );
}
