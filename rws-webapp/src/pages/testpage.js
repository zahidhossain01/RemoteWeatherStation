import {useRouter} from 'next/router'
import {useState, useMemo, useEffect} from 'react'
// import {Chart} from 'react-charts'
// https://github.com/TanStack/react-charts/issues/304
import dynamic from 'next/dynamic';
const Chart = dynamic(() => import("react-charts").then((mod) => mod.Chart), {ssr: false,});

import { ResizableBox } from 'react-resizable';

import { firestore } from '@/firebase';
import { collection, getDocs } from 'firebase/firestore';

export default function TestPage(){

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

    const data = useMemo(
        () => (
            [
                {
                    label: "Temperature",
                    data: weather_data.map((entry) => (
                        {date: entry['time'], temp: entry['temp']}
                    ))
                }
            ]
        ),
        [weather_data]
    );
    

    const primaryAxis = useMemo(
        () => (
            {
                getValue: (datum) => datum.date
            }
        ),
        []
    );

    const secondaryAxes = useMemo(
        () => [
            {
                getValue: (datum) => datum.temp,
                elementType: 'line'
            },
            {
                getValue: (datum) => datum.humidity,
                elementType: 'line'
            },
        ],
        []
    );
    

    return(
        <>
            <h1>test chart page title</h1>
            <button onClick={() => router.push('/')}>back home</button>
            <button onClick={test_firestore}>Load Firestore</button>


            {/* TEST CHARTING STUFF */}
            <ResizableBox height={500} width={500}>
                <Chart
                    options={{
                        data,
                        primaryAxis,
                        secondaryAxes
                    }}
                />
            </ResizableBox>


        </>
    );
}
