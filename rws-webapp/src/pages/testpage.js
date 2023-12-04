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
        {time: "00:00:00", temp: 0, humidity: 0}
    ]);

    const getWeatherDataTest = async () => {
        const sensorDataRef = collection(firestore, "sensor_data");
        const snapshot = await getDocs(sensorDataRef);
        const sensorData = snapshot.docs.map((doc) => doc.data());
        console.log(sensorData.length + " data points received");
        return sensorData;
    }

    const test_firestore = async () => {
        const weatherData = await getWeatherDataTest();
        set_weather_data(weatherData);
        console.log(weatherData);
    }

    useEffect(() => {
        getWeatherDataTest().then((data) => {
            set_weather_data(data);
        });
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


    const data = useMemo(
        () => (
            [
                {
                    label: "Temperature",
                    data: weather_data.map((entry) => (
                        {date: new Date(entry['time']), temp: entry['temp']}
                    ))
                    // TODO: SORT
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
    

    return(
        <div>
            <h1>test chart page title</h1>
            <button onClick={() => router.push('/')}>back home</button>
            <button onClick={test_firestore}>Load Firestore</button>


            {/* TEST CHARTING STUFF */}
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
