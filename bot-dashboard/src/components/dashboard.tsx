'use client'

import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card"
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer } from 'recharts'
import { ScrollArea } from "@/components/ui/scroll-area"
import { Table, TableBody, TableCell, TableHead, TableHeader, TableRow } from "@/components/ui/table"

// Placeholder data for the account balance chart
const accountData = Array.from({ length: 30 }, (_, i) => ({
  date: new Date(Date.now() - (29 - i) * 24 * 60 * 60 * 1000).toISOString().split('T')[0],
  balance: Math.floor(Math.random() * (10000 - 5000 + 1) + 5000)
}))

// Placeholder data for recent trades
const recentTrades = [
  { id: 1, date: '2023-06-01', pair: 'BTC/USDT', type: 'Buy', amount: 0.1, price: 27500 },
  { id: 2, date: '2023-06-01', pair: 'ETH/USDT', type: 'Sell', amount: 1.5, price: 1900 },
  { id: 3, date: '2023-05-31', pair: 'ADA/USDT', type: 'Buy', amount: 1000, price: 0.38 },
  { id: 4, date: '2023-05-31', pair: 'XRP/USDT', type: 'Sell', amount: 500, price: 0.52 },
  { id: 5, date: '2023-05-30', pair: 'DOT/USDT', type: 'Buy', amount: 50, price: 5.2 },
]

export default function DashboardComponent() {
  return (
    <div className="p-6 space-y-6 bg-[#1a1f2e] text-gray-200 min-h-screen">
      <h1 className="text-3xl font-bold text-white">Dashboard</h1>
      <div className="grid gap-6 md:grid-cols-2">
        <Card className="bg-[#232936] border-0 shadow-lg">
          <CardHeader>
            <CardTitle className="text-white">Account Balance (Last 30 Days)</CardTitle>
          </CardHeader>
          <CardContent>
            <div className="h-[300px]">
              <ResponsiveContainer width="100%" height="100%">
                <LineChart data={accountData}>
                  <CartesianGrid strokeDasharray="3 3" stroke="#3a4256" />
                  <XAxis dataKey="date" stroke="#8f9bb3" />
                  <YAxis stroke="#8f9bb3" />
                  <Tooltip 
                    contentStyle={{ backgroundColor: '#2a3142', border: 'none' }}
                    labelStyle={{ color: '#8f9bb3' }}
                  />
                  <Line type="monotone" dataKey="balance" stroke="#3366ff" />
                </LineChart>
              </ResponsiveContainer>
            </div>
          </CardContent>
        </Card>
        <Card className="bg-[#232936] border-0 shadow-lg">
          <CardHeader>
            <CardTitle className="text-white">Recent Trades</CardTitle>
          </CardHeader>
          <CardContent>
            <ScrollArea className="h-[300px]">
              <Table>
                <TableHeader>
                  <TableRow className="border-b border-gray-700">
                    <TableHead className="text-gray-400">Date</TableHead>
                    <TableHead className="text-gray-400">Pair</TableHead>
                    <TableHead className="text-gray-400">Type</TableHead>
                    <TableHead className="text-gray-400">Amount</TableHead>
                    <TableHead className="text-gray-400">Price</TableHead>
                  </TableRow>
                </TableHeader>
                <TableBody>
                  {recentTrades.map((trade) => (
                    <TableRow key={trade.id} className="border-b border-gray-700">
                      <TableCell className="text-gray-300">{trade.date}</TableCell>
                      <TableCell className="text-gray-300">{trade.pair}</TableCell>
                      <TableCell className={trade.type === 'Buy' ? 'text-green-400' : 'text-red-400'}>
                        {trade.type}
                      </TableCell>
                      <TableCell className="text-gray-300">{trade.amount}</TableCell>
                      <TableCell className="text-gray-300">${trade.price.toFixed(2)}</TableCell>
                    </TableRow>
                  ))}
                </TableBody>
              </Table>
            </ScrollArea>
          </CardContent>
        </Card>
      </div>
    </div>
  )
}