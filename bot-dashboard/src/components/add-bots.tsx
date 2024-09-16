'use client'

import React, { useState } from 'react';
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
  side: string;
  baseSizeType: string;
  orderType: string;
  baseOrderSize: number;
  leverage: number;
}

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

export default function TradingBotDashboard() {
  const [bots, setBots] = useState<Bot[]>([]);
  const [newBot, setNewBot] = useState<Bot>({
    id: '',
    type: 'spot',
    tradingPair: '',
    side: 'BUY',
    baseSizeType: '',
    orderType: 'MARKET',
    baseOrderSize: 0,
    leverage: 1,
  });
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const handleInputChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const { name, value } = e.target;
    setNewBot(prev => ({ ...prev, [name]: value }));
  };

  const handleOrderValueChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    const value = Number(event.target.value); // Convert to number
    setNewBot(prev => ({ ...prev, baseOrderSize: value }));
  };

  const handleTypeChange = (name: keyof Bot, value: string | number) => {
    setNewBot(prev => ({ ...prev, [name]: value }));
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
        side: 'BUY',
        baseSizeType: '',
        orderType: 'MARKET',
        baseOrderSize: 0,
        leverage: 1,
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
        <h1 className="text-3xl font-bold mb-6 text-gray-100">Add New Trading Bot</h1>
        
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
                <Label htmlFor="side" className="text-gray-200">Bot Side (Long or Short)</Label>
                <Select onValueChange={(value) => handleTypeChange('side', value)} value={newBot.side}>
                  <SelectTrigger className="w-full bg-gray-700 text-gray-100 border-gray-600">
                    <SelectValue placeholder="Select bot side" />
                  </SelectTrigger>
                  <SelectContent className="bg-gray-700 text-gray-100 border-gray-600">
                    <SelectItem value="BUY">BUY</SelectItem>
                    <SelectItem value="SELL">SELL</SelectItem>
                  </SelectContent>
                </Select>
              </div>
              <div>
                <Label htmlFor="type" className="text-gray-200">Bot Type</Label>
                <Select onValueChange={(value) => handleTypeChange('type', value)} value={newBot.type}>
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
                <Label htmlFor="baseSizeType" className="text-gray-200">Order in: USD Value/Percentage of Account</Label>
                <Select onValueChange={(value) => handleTypeChange('baseSizeType', value)} value={newBot.baseSizeType}>
                  <SelectTrigger className="w-full bg-gray-700 text-gray-100 border-gray-600">
                    <SelectValue placeholder="Select USD or Percentage" />
                  </SelectTrigger>
                  <SelectContent className="bg-gray-700 text-gray-100 border-gray-600">
                    <SelectItem value="USD">USD</SelectItem>
                    <SelectItem value="Percentage">Percentage</SelectItem>
                  </SelectContent>
                </Select>
              </div>
              <div>
                <Label htmlFor="orderType" className="text-gray-200">Market or Limit order</Label>
                <Select onValueChange={(value) => handleTypeChange('orderType', value)} value={newBot.orderType}>
                  <SelectTrigger className="w-full bg-gray-700 text-gray-100 border-gray-600">
                    <SelectValue placeholder="Select Market or Limit order" />
                  </SelectTrigger>
                  <SelectContent className="bg-gray-700 text-gray-100 border-gray-600">
                    <SelectItem value="MARKET">Market</SelectItem>
                    <SelectItem value="LIMIT">Limit</SelectItem>
                  </SelectContent>
                </Select>
              </div>

              <div>
                <Label htmlFor="baseOrderSize" className="text-gray-200">Order Size ({newBot.baseSizeType})</Label>
                <Input
                  id="baseOrderSize"
                  name="baseOrderSize"
                  value={newBot.baseOrderSize}
                  onChange={handleOrderValueChange}
                  required
                  className="bg-gray-700 text-gray-100 border-gray-600 focus:border-blue-500"
                />
              </div>
             
              <div>
                <Label htmlFor="leverage" className="text-gray-200">Leverage</Label>
                <div className="flex items-center space-x-4">
                  <CustomSlider
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
                
              </div>
              <Button type="submit" disabled={isLoading} className="bg-blue-600 hover:bg-blue-700 text-white">
                {isLoading ? 'Saving...' : 'Save Bot'}
              </Button>
            </form>
          </CardContent>
        </Card>

        {error && <p className="text-red-400 mb-4">{error}</p>}
      </div>
    </div>
  );
}
