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
        { time: "00:00:00", temp: 0, humidity: 0 }
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

    // const data_series = useMemo(
    //     () => (
    //         [
    //             {
    //                 label: "Test Series Label",
    //                 data: [
    //                     {date: "2023-04-15T23:23:31.157966Z", temp: 75},
    //                     {date: "2023-04-16T00:37:25.605266Z", temp: 90},
    //                     {date: "2023-04-16T00:38:25.605266Z", temp: 90},
    //                     {date: "2023-04-16T00:39:25.605266Z", temp: 90},
    //                     {date: "2023-04-16T00:40:25.605266Z", temp: 90},
    //                     {date: "2023-04-16T00:41:25.605266Z", temp: 90},
    //                     {date: "2023-04-16T00:42:25.605266Z", temp: 90},
    //                     {date: "2023-04-16T00:43:25.605266Z", temp: 90},
    //                     {date: "2023-04-16T00:44:25.605266Z", temp: 90}
    //                 ]
    //             }
    //         ]
    //     )
    // );

    // const data = useMemo(
    //     () => (
    //         [
    //             {
    //                 label: "Test Series Label",
    //                 data: [
    //                     {date: "00:37:25", temp: 90},
    //                     {date: "00:38:25", temp: 80},
    //                     {date: "00:39:25", temp: 90},
    //                     {date: "00:40:25", temp: 90},
    //                     {date: "00:41:25", temp: 90},
    //                     {date: "00:42:25", temp: 90},
    //                     {date: "00:43:25", temp: 90},
    //                     {date: "00:44:25", temp: 120}
    //                 ]
    //             }
    //         ]
    //     )
    // );

    // TODO: SHOULD DEFINITELY NOT BE JUST DOING DATE(ENTRY['TIME']) LATER ON BECAUSE IT CONVERTS TO THE DAY ON WHICH YOU ACCESS

    const data = useMemo(
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


    const primaryAxis = useMemo(
        () => (
            {
                type: 'linear',
                getValue: (datum) => datum.date
            }
        ),
        []
    );

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
            <h1>WeatherStation</h1>
            <button onClick={() => router.push('/')}>back home</button>
            <button onClick={test_firestore}>Load Firestore</button>


            {/* TEST CHARTING STUFF */}
            <h2>Temperature</h2>
            <ResizableBox height={750} width={750}>
                <Chart
                    options={{
                        data,
                        primaryAxis,
                        secondaryAxes
                    }}
                />
            </ResizableBox>


        </div>
    );
}
