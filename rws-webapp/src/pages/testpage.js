import { useRouter } from 'next/router'
import { useState, useMemo, useEffect } from 'react'
// import {Chart} from 'react-charts'
// https://github.com/TanStack/react-charts/issues/304
import dynamic from 'next/dynamic';
const Chart = dynamic(() => import("react-charts").then((mod) => mod.Chart), { ssr: false, });

import { ResizableBox } from 'react-resizable';

import { firestore } from '@/firebase';
import { collection, getDocs, query, orderBy, limit, onSnapshot } from 'firebase/firestore';

export default function TestPage() {

    const router = useRouter();
    const [weather_data, set_weather_data] = useState([
        { time: "00:00:00", temp: 0, humidity: 0, pressure: 0, rainfall: 0, speed: 0, pma: 0, pmb: 0, pmc: 0 }
    ]);

    const getWeatherDataTest = async () => {
        const sensorDataRef = collection(firestore, "sensor_data");
        const queryRef = query(sensorDataRef, orderBy("timestamp", "desc"), limit(5));
        const snapshot = await getDocs(queryRef);
        const sensorData = snapshot.docs.map((doc) => doc.data());

        console.log(sensorData.length + " most recent data points received");
        return sensorData;
    }

    const test_firestore = async () => {
        const weatherData = await getWeatherDataTest();
        set_weather_data(weatherData);
        console.log(weatherData);
    }

    useEffect(() => {
        const sensorDataRef = collection(firestore, "sensor_data");
        const queryRef = query(sensorDataRef, orderBy("timestamp", "desc"), limit(5));

        // This sets up the real-time listener
        const unsubscribe = onSnapshot(queryRef, (snapshot) => {
            const sensorData = snapshot.docs.map(doc => doc.data());
            set_weather_data(sensorData);
        });

        // Cleanup function to unsubscribe from the listener when the component unmounts
        return () => unsubscribe();
    }, []);

    // in future, consider TypeScript to more clearly label our datatypes for axes
    // https://react-charts.tanstack.com/docs/api | https://react-charts.tanstack.com/docs/getting-started
    // primary and secondary names are just convention

    // const data = useMemo(
    //     () => (
    //         [
    //             {
    //                 label: "Test Series Label",
    //                 data: [
    //                     {time: new Date("2023-04-16T01:38:25.605266Z"), temp: 60},
    //                     {time: new Date("2023-04-16T02:39:25.605266Z"), temp: 85},
    //                     {time: new Date("2023-04-16T03:40:25.605266Z"), temp: 93},
    //                     {time: new Date("2023-04-16T07:44:25.605266Z"), temp: 100},
    //                     {time: new Date("2023-04-16T04:41:25.605266Z"), temp: 104},
    //                     {time: new Date("2023-04-15T23:23:31.157966Z"), temp: 75},
    //                     {time: new Date("2023-04-16T00:37:25.605266Z"), temp: 90},
    //                     {time: new Date("2023-04-16T05:42:25.605266Z"), temp: 90},
    //                     {time: new Date("2023-04-16T06:43:25.605266Z"), temp: 95}
    //                 ]
    //             }
    //         ]
    //     )
    // );
    // TODO: ^ above is unsorted time, how to get react-charts to sort? or just presort by date...?

    // const data = useMemo(
    //     () => (
    //         [
    //             {
    //                 label: "Test Series Label",
    //                 data: [
    //                     {time: "00:37:25", temp: 90},
    //                     {time: "00:38:25", temp: 80},
    //                     {time: "00:39:25", temp: 90},
    //                     {time: "00:40:25", temp: 90},
    //                     {time: "00:41:25", temp: 90},
    //                     {time: "00:42:25", temp: 90},
    //                     {time: "00:43:25", temp: 90},
    //                     {time: "00:44:25", temp: 120}
    //                 ]
    //             }
    //         ]
    //     )
    // );


    const temperatureData = useMemo(
        () => (
            [
                {
                    label: "Temperature",
                    data: weather_data.slice().reverse().map((entry) => (
                        { date: entry['time'], temp: entry['temp'] }
                    ))
                },
                // {
                //     label: "Humidity",
                //     data: weather_data.map((entry) => (
                //         {date: entry['time'], humidity: entry['humidity']}
                //     ))
                // }
            ]
        ),
        [weather_data]
    );


    const temperaturePrimaryAxis = useMemo(
        () => (
            {
                type: 'linear',
                getValue: (datum) => datum.date
            }
        ),
        []
    );

    const temperatureSecondaryAxes = useMemo(
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

    //Humidity data and axis
    const humidityData = useMemo(
        () => (
            [
                {
                    label: "Humidity",
                    data: weather_data.slice().reverse().map((entry) => (
                        { date: entry['time'], humidity: entry['humidity'] }
                    ))
                }
            ]
        ),
        [weather_data]
    );
    const humidityPrimaryAxis = useMemo(
        () => (
            {
                type: 'linear',
                getValue: (datum) => datum.date
            }
        ),
        []
    );
    
    const humiditySecondaryAxes = useMemo(
        () => [
            {
                getValue: (datum) => datum.humidity,
                elementType: 'line',
                range: [0, 100] // Humidity range from 0 to 100%
            }
        ],
        []
    );
    
    //Pressure data and axis
    const pressureData = useMemo(
        () => (
            [
                {
                    label: "Pressure",
                    data: weather_data.slice().reverse().map((entry) => (
                        { date: entry['time'], pressure: entry['pressure'] }
                    ))
                }
            ]
        ),
        [weather_data]
    );
    const pressurePrimaryAxis = useMemo(
        () => (
            {
                type: 'linear',
                getValue: (datum) => datum.date
            }
        ),
        []
    );
    
    const pressureSecondaryAxes = useMemo(
        () => [
            {
                getValue: (datum) => datum.pressure,
                elementType: 'line',
                range: [0, 100] // Pressure range from 0 to 100 (unit?)
            }
        ],
        []
    );

    //Wind speed data and axis
    const speedData = useMemo(
        () => (
            [
                {
                    label: "Wind Speed",
                    data: weather_data.slice().reverse().map((entry) => (
                        { date: entry['time'], speed: entry['speed'] }
                    ))
                }
            ]
        ),
        [weather_data]
    );
    const speedPrimaryAxis = useMemo(
        () => (
            {
                type: 'linear',
                getValue: (datum) => datum.date
            }
        ),
        []
    );
    
    const speedSecondaryAxes = useMemo(
        () => [
            {
                getValue: (datum) => datum.speed,
                elementType: 'line',
                range: [0, 100] // speed range from 0 to 100 (units?)
            }
        ],
        []
    );

    //Rainfall data and axis
    const rainfallData = useMemo(
        () => (
            [
                {
                    label: "Rainfall",
                    data: weather_data.slice().reverse().map((entry) => (
                        { date: entry['time'], rainfall: entry['rain'] }
                    ))
                }
            ]
        ),
        [weather_data]
    );
    const rainfallPrimaryAxis = useMemo(
        () => (
            {
                type: 'linear',
                getValue: (datum) => datum.date
            }
        ),
        []
    );
    
    const rainfallSecondaryAxes = useMemo(
        () => [
            {
                getValue: (datum) => datum.rainfall,
                elementType: 'line',
                range: [0, 100] // Rainfall range from 0 to 100 (units?)
            }
        ],
        []
    );

    //PM 1.0 data and axis
    const pmaData = useMemo(
        () => (
            [
                {
                    label: "PM 1.0",
                    data: weather_data.slice().reverse().map((entry) => (
                        { date: entry['time'], pma: entry['pms_1_0'] }
                    ))
                }
            ]
        ),
        [weather_data]
    );
    const pmaPrimaryAxis = useMemo(
        () => (
            {
                type: 'linear',
                getValue: (datum) => datum.date
            }
        ),
        []
    );
    
    const pmaSecondaryAxes = useMemo(
        () => [
            {
                getValue: (datum) => datum.pma,
                elementType: 'line',
                range: [0, 100000] // pm 1.0 range from 0 to 1000000(ug/m3)
            }
        ],
        []
    );

    //PM 2.5 data and axis
    const pmbData = useMemo(
        () => (
            [
                {
                    label: "PM 2.5",
                    data: weather_data.slice().reverse().map((entry) => (
                        { date: entry['time'], pmb: entry['pms_2_5'] }
                    ))
                }
            ]
        ),
        [weather_data]
    );
    const pmbPrimaryAxis = useMemo(
        () => (
            {
                type: 'linear',
                getValue: (datum) => datum.date
            }
        ),
        []
    );
    
    const pmbSecondaryAxes = useMemo(
        () => [
            {
                getValue: (datum) => datum.pmb,
                elementType: 'line',
                range: [0, 100000] // pm 2.5 range from 0 to 1000000(ug/m3)
            }
        ],
        []
    );

    //PM 10.0 data and axis
    const pmcData = useMemo(
        () => (
            [
                {
                    label: "PM 10",
                    data: weather_data.slice().reverse().map((entry) => (
                        { date: entry['time'], pmc: entry['pms_10_0'] }
                    ))
                }
            ]
        ),
        [weather_data]
    );
    const pmcPrimaryAxis = useMemo(
        () => (
            {
                type: 'linear',
                getValue: (datum) => datum.date
            }
        ),
        []
    );
    
    const pmcSecondaryAxes = useMemo(
        () => [
            {
                getValue: (datum) => datum.pmc,
                elementType: 'line',
                range: [0, 100000] // pm 1.0 range from 0 to 1000000(ug/m3)
            }
        ],
        []
    );

    return (
        <div>
            <h1>WeatherStation</h1>
            <button onClick={() => router.push('/')}>back home</button>
            <button onClick={test_firestore}>Load Firestore</button>
    
            {/* Flex container for charts */}
            <div style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', gap: '20px' }}>
                {/* Temperature Chart */}
                <div>
                    <h2>Temperature</h2>
                    <ResizableBox height={750} width={750}>
                        <Chart
                            options={{
                                data: temperatureData, 
                                primaryAxis: temperaturePrimaryAxis, 
                                secondaryAxes: temperatureSecondaryAxes
                            }}
                        />
                    </ResizableBox>
                </div>
    
                {/* Humidity Chart */}
                <div>
                    <h2>Humidity</h2>
                    <ResizableBox height={750} width={750}>
                        <Chart
                            options={{
                                data: humidityData,
                                primaryAxis: humidityPrimaryAxis,
                                secondaryAxes: humiditySecondaryAxes
                            }}
                        />
                    </ResizableBox>
                </div>
            </div>

            {/* Flex container for charts */}
            <div style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', gap: '20px' }}>
                {/* Pressure Chart */}
                <div>
                    <h2>Pressure</h2>
                    <ResizableBox height={750} width={750}>
                        <Chart
                            options={{
                                data: pressureData, 
                                primaryAxis: pressurePrimaryAxis, 
                                secondaryAxes: pressureSecondaryAxes
                            }}
                        />
                    </ResizableBox>
                </div>
    
                {/* Wind Speed Chart */}
                <div>
                    <h2>Wind Speed</h2>
                    <ResizableBox height={750} width={750}>
                        <Chart
                            options={{
                                data: speedData,
                                primaryAxis: speedPrimaryAxis,
                                secondaryAxes: speedSecondaryAxes
                            }}
                        />
                    </ResizableBox>
                </div>
            </div>

            {/* Flex container for charts */}
            <div style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', gap: '20px' }}>
                {/* Rainfall Chart */}
                <div>
                    <h2>Rainfall</h2>
                    <ResizableBox height={750} width={750}>
                        <Chart
                            options={{
                                data: rainfallData, 
                                primaryAxis: rainfallPrimaryAxis, 
                                secondaryAxes: rainfallSecondaryAxes
                            }}
                        />
                    </ResizableBox>
                </div>
    
                {/* PM 1.0 Chart */}
                <div>
                    <h2>PM 1.0</h2>
                    <ResizableBox height={750} width={750}>
                        <Chart
                            options={{
                                data: pmaData,
                                primaryAxis: pmaPrimaryAxis,
                                secondaryAxes: pmaSecondaryAxes
                            }}
                        />
                    </ResizableBox>
                </div>
            </div>

            {/* Flex container for charts */}
            <div style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', gap: '20px' }}>
                {/* PM 2.5 Chart */}
                <div>
                    <h2>PM 2.5</h2>
                    <ResizableBox height={750} width={750}>
                        <Chart
                            options={{
                                data: pmbData, 
                                primaryAxis: pmbPrimaryAxis, 
                                secondaryAxes: pmbSecondaryAxes
                            }}
                        />
                    </ResizableBox>
                </div>
    
                {/* PM 10.0 Chart */}
                <div>
                    <h2>PM 10.0</h2>
                    <ResizableBox height={750} width={750}>
                        <Chart
                            options={{
                                data: pmcData,
                                primaryAxis: pmcPrimaryAxis,
                                secondaryAxes: pmcSecondaryAxes
                            }}
                        />
                    </ResizableBox>
                </div>
            </div>
        </div>
    );
}
