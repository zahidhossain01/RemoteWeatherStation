import { useRouter } from 'next/router'
import { useState, useMemo } from 'react'
import dynamic from 'next/dynamic'
import { firestore } from '@/firebase'
import { collection, query, where, getDocs } from 'firebase/firestore'

const Chart = dynamic(() => import("react-charts").then((mod) => mod.Chart), { ssr: false })

export default function WeatherChartPage() {
    const router = useRouter()
    const [weatherData, setWeatherData] = useState([])
    const [timeRange, setTimeRange] = useState({ start: '', end: '' })
    const isTimeRangeSelected = timeRange.start && timeRange.end;

    const fetchWeatherData = async () => {
        if (!timeRange.start || !timeRange.end) {
            console.log("Please select both start and end times.");
            return;
        }
        
        // Parse the time range to Date objects
        const startDate = new Date(timeRange.start);
        const endDate = new Date(timeRange.end);
        
        const sensorDataRef = collection(firestore, "sensor_data");
        // Create a query with the time range
        const queryRef = query(sensorDataRef, 
            where("time", ">=", startDate.toISOString()), 
            where("time", "<=", endDate.toISOString())
        );
        const snapshot = await getDocs(queryRef);
        const sensorData = snapshot.docs.map(doc => {
            const data = doc.data();
            
            // Parse the Firestore time string to a Date object
            // Adjust the parsing to match the format of the time string from Firestore
            const time = new Date(data.time);
            
            return {
                ...data,
                time: time // Use the parsed Date object
            };
        });
    
        setWeatherData(sensorData);
    };
    

    const data = useMemo(
        () => (
            [
                {
                    label: "Temperature",
                    data: weatherData.slice().reverse().map((entry) => (
                        { date: entry['time'], temp: entry['temp'] }
                    ))
                },
            ]
        ),
        [weatherData]
    );

    // Test Data
    // const data = useMemo(() => [
    //     {
    //         label: "Temperature",
    //         data: [
    //             { date: new Date('2023-01-01T00:00:00'), temp: 20 },
    //             { date: new Date('2023-01-02T00:00:00'), temp: 22 },
    //             // ... more test data ...
    //         ]
    //     }
    // ], []);
    
    const primaryAxis = useMemo(() => ({
        type: 'time',
        getValue: datum => datum.date
    }), []);
    

    const secondaryAxes = useMemo(
        () => [
            {
                getValue: (datum) => datum.temp,
                elementType: 'line',
                range: [0, 200]
            },
            // {
            //     getValue: (datum) => datum.humidity,
            //     elementType: 'bar',
            //     range: [0, 100]
            // }
        ],
        []
    );

    return (
        <div>
            <h1>Weather Data Chart</h1>
            {/* ... other components ... */}

            <div>
                <input type="datetime-local" value={timeRange.start} onChange={(e) => setTimeRange({ ...timeRange, start: e.target.value })} />
                <input type="datetime-local" value={timeRange.end} onChange={(e) => setTimeRange({ ...timeRange, end: e.target.value })} />
                <button onClick={fetchWeatherData} disabled={!isTimeRangeSelected}>Load Data</button>
            </div>

            {isTimeRangeSelected && weatherData.length > 0 && (
                <div>
                    <h2>Temperature Chart</h2>
                    <div style={{ height: '500px', width: '100%' }}>
                        <Chart
                            options={{
                                data,
                                primaryAxis,
                                secondaryAxes
                            }}
                        />
                    </div>
                </div>
            )}
        </div>
    )
}
