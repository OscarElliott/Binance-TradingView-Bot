import React, { useEffect, useState } from 'react';
import { Button } from "@/components/ui/button"
import { Input } from "@/components/ui/input"
import { Label } from "@/components/ui/label"
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card"
import { Select, SelectContent, SelectItem, SelectTrigger, SelectValue } from "@/components/ui/select"
import { CustomSlider } from "@/components/ui/custom-slider"
import { Bot } from '@/lib/types'

interface Bot {
  id: string;
  type: string;
  tradingPair: string;
  leverage: number;
}

const fetchBotsFromAPI = async (): Promise<Bot[]> => {
    try 
    {
      const response = await fetch('http://localhost:9080/getbots', {
        method: 'GET',
        mode: "cors",
      });
      if (!response.ok) {
        throw new Error('Failed to fetch bots');
      }
      // Parse the response JSON
      const data = await response.json();
  
      // Check if the status is success and if bots are returned
      if (data.status === 'success' && Array.isArray(data.bots)) 
      {
        // Ensure each bot in the array conforms to the Bot type
        const bots: Bot[] = data.bots.map((bot: any) => {
            if (typeof bot.id === 'string' &&
                typeof bot.type === 'string' &&
                typeof bot.tradingPair === 'string' &&
                typeof bot.leverage === 'number') {
                return bot as Bot;
            } else {
                console.log(bot)
                console.log(typeof bot.id === 'string')
                console.log(typeof bot.type === 'string')
                console.log(typeof bot.tradingPair === 'string')
                console.log(typeof bot.leverage === 'number')
                throw new Error('Invalid bot data');
            }
        });
        return bots;
      }
      else {
        throw new Error('Invalid response structure');
      }
    } catch (error) {
        console.error('Error fetching bots:', error);
        // Optionally, you can rethrow the error or handle it based on your needs
        throw error;
    }
};

const saveBotToAPI = async (bot: Bot): Promise<Bot> => {
    try {
      console.log('Sending bot data:', bot);
      const response = await fetch('http://localhost:9080/addbot', {
        method: 'POST',
        mode: "cors",
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(bot),
      });
      console.log('Final result:', response);
  
      if (!response.ok) {
        const errorText = await response.text(); // Retrieve error text
        throw new Error(`Failed to save bot: ${errorText}`);
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
    <div className="min-h-screen bg-gray-900 text-gray-100 p-8">
      <div className="max-w-6xl mx-auto">
        <h1 className="text-4xl font-bold mb-8 text-gray-100">Manage Bots</h1>

        <div className="grid grid-cols-1 md:grid-cols-2 gap-8">
          <Card className="bg-gray-800 border-gray-700">
            <CardHeader>
              <CardTitle className="text-2xl text-gray-100">Select a Bot</CardTitle>
              <CardDescription className="text-gray-400">Choose a bot to edit.</CardDescription>
            </CardHeader>
            <CardContent>
              {bots.length === 0 ? (
                <p className="text-gray-400">No bots available.</p>
              ) : (
                <div className="grid grid-cols-1 sm:grid-cols-2 gap-4">
                  {bots.map(bot => (
                    <Card
                      key={bot.id}
                      className={`bg-gray-750 hover:bg-gray-700 transition-colors cursor-pointer ${
                        selectedBot?.id === bot.id ? 'ring-2 ring-blue-500' : ''
                      }`}
                      onClick={() => handleBotSelect(bot)}
                    >
                      <CardContent className="p-4 flex items-center space-x-4">
                        <div>
                            <img className="BOT_IMG" src="src/assets/Bot-img.png" alt="BOT" />
                        </div>
                        <div>
                          <h3 className="font-semibold text-lg">{bot.id}</h3>
                          <p className="text-sm text-gray-400">{bot.tradingPair}</p>
                        </div>
                      </CardContent>
                    </Card>
                  ))}
                </div>
              )}
            </CardContent>
          </Card>

          {selectedBot && (
            <Card className="bg-gray-800 border-gray-700">
              <CardHeader>
                <CardTitle className="text-2xl text-gray-100">Edit Bot</CardTitle>
                <CardDescription className="text-gray-400">Edit the selected bot's details.</CardDescription>
              </CardHeader>
              <CardContent>
                <form onSubmit={handleSubmit} className="space-y-6">
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
                  <Button type="submit" disabled={isLoading} className="w-full bg-blue-600 hover:bg-blue-700 text-white">
                    {isLoading ? 'Saving...' : 'Save Bot'}
                  </Button>
                </form>
              </CardContent>
            </Card>
          )}
        </div>

        {error && <p className="text-red-400 mt-4">{error}</p>}
      </div>
    </div>
  )
}