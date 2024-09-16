import React, { useState, useEffect } from 'react';
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { Label } from "@/components/ui/label";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";

interface User {
  email: string;
  apiKey: string;
  apiSecret: string;
}

export default function Settings() {
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [userDetails, setUserDetails] = useState<User>({
    email: '',
    apiKey: '',
    apiSecret: '***************', // Masked API secret
  });

  // Fetch user settings from the backend
  useEffect(() => {
    const fetchUserDetails = async () => {
      setIsLoading(true);
      try {
        const response = await fetch('http://localhost:9080/getSettings', {
            method: 'GET',
            mode: "cors",
        });
        if (!response.ok) {
          throw new Error('Failed to fetch user settings');
        }
        const data = await response.json();
        console.log('Final result:', response);
        setUserDetails({
          email: data.Email || '',
          apiKey: data.apiKey || '',
          apiSecret: '***************', // Keep secret masked
        });
      } catch (error) {
        setError('Failed to fetch user details. Please try again.');
      } finally {
        setIsLoading(false);
      }
    };

    fetchUserDetails();
  }, []);

  // Handle input changes
  const handleInputChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const { name, value } = e.target;
    setUserDetails(prev => ({ ...prev, [name]: value }));
  };

  // Handle form 
  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setIsLoading(true);
    setError(null);
    try {
      const response = await fetch('http://localhost:9080/postSettings', { 
        method: 'POST',
        mode: "cors",
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          Email: userDetails.email,
          binance_api_key: userDetails.apiKey,
          binance_api_secret: userDetails.apiSecret,
        }),
      });
      if (!response.ok) {
        throw new Error('Failed to update user settings');
      }
      const data = await response.json();
      // Update the state or notify user of success
      setUserDetails(prev => ({ ...prev, apiSecret: '***************' }));
    } catch (error) {
      setError('Failed to save user details. Please try again.');
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <div className="min-h-screen bg-gray-900 text-gray-100 p-4">
      <div className="container mx-auto">
        <h1 className="text-3xl font-bold mb-6 text-gray-100">Settings</h1>
        
        <Card className="mb-8 bg-gray-800 border-gray-700">
          <CardHeader>
            <CardTitle className="text-gray-100">User Settings</CardTitle>
            <CardDescription className="text-gray-400">Update your email, API key, and API secret.</CardDescription>
          </CardHeader>
          <CardContent>
            <form onSubmit={handleSubmit} className="space-y-4">
              <div>
                <Label htmlFor="email" className="text-gray-200">Email</Label>
                <Input
                  id="email"
                  name="email"
                  value={userDetails.email}
                  onChange={handleInputChange}
                  required
                  className="bg-gray-700 text-gray-100 border-gray-600 focus:border-blue-500"
                />
              </div>
              <div>
                <Label htmlFor="apiKey" className="text-gray-200">API Key</Label>
                <Input
                  id="apiKey"
                  name="apiKey"
                  value={userDetails.apiKey}
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
                  value={userDetails.apiSecret}
                  onChange={handleInputChange}
                  required
                  type="password" // Ensure the API secret is not visible
                  className="bg-gray-700 text-gray-100 border-gray-600 focus:border-blue-500"
                />
              </div>
              <Button type="submit" disabled={isLoading} className="bg-blue-600 hover:bg-blue-700 text-white">
                {isLoading ? 'Saving...' : 'Save Settings'}
              </Button>
            </form>
          </CardContent>
        </Card>

        {error && <p className="text-red-400 mb-4">{error}</p>}
      </div>
    </div>
  );
}
