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
    const [selectedDataType, setSelectedDataType] = useState('temp'); // New state for selected data type

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
        () => [
            {
                label: selectedDataType.charAt(0).toUpperCase() + selectedDataType.slice(1), // Dynamic label based on selected data type
                data: weatherData.slice().reverse().map((entry) => (
                    { date: entry['time'], value: entry[selectedDataType] } // Use selected data type for data
                ))
            }
        ],
        [weatherData, selectedDataType]
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
                getValue: (datum) => datum.value,
                elementType: 'line',
                // Adjust range based on selectedDataType if necessary
            }
        ],
        [selectedDataType]
    );

    return (
        <div>
            <h1>Weather Data Chart</h1>
            {/* ... other components ... */}

            <div>
                <input type="datetime-local" value={timeRange.start} onChange={(e) => setTimeRange({ ...timeRange, start: e.target.value })} />
                <input type="datetime-local" value={timeRange.end} onChange={(e) => setTimeRange({ ...timeRange, end: e.target.value })} />
                <button onClick={fetchWeatherData} disabled={!isTimeRangeSelected}>Load Data</button>
                {/* Dropdown to select data type */}
                <select value={selectedDataType} onChange={(e) => setSelectedDataType(e.target.value)}>
                    <option value="temp">Temperature</option>
                    <option value="humidity">Humidity</option>
                    <option value="pressure">Pressure</option>
                    <option value="rain">Rain</option>
                    <option value="speed">Speed</option>
                    <option value="pms_10_0">PMS 10.0</option>
                    <option value="pms_1_0">PMS 1.0</option>
                    <option value="pms_2_5">PMS 2.5</option>
                </select>

                <button onClick={fetchWeatherData} disabled={!isTimeRangeSelected}>Load Data</button>
            </div>

            {/* Chart rendering */}
            {isTimeRangeSelected && weatherData.length > 0 && (
                <div>
                    <h2>{`${selectedDataType.charAt(0).toUpperCase() + selectedDataType.slice(1)} Chart`}</h2>
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
