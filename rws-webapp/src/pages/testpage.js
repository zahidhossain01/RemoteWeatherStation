import {useRouter} from 'next/router'
import {useState, useMemo, useEffect} from 'react'
// import {Chart} from 'react-charts'
// https://github.com/TanStack/react-charts/issues/304
import dynamic from 'next/dynamic';
const Chart = dynamic(() => import("react-charts").then((mod) => mod.Chart), {ssr: false,});

// https://firebase.google.com/docs/firestore/security/insecure-rules#mixed-public-and-private-access

import { ResizableBox } from 'react-resizable';

import { firestore } from '@/firebase';
import { collection, getDocs } from 'firebase/firestore';

export default function TestPage(){

    const router = useRouter();
    const [weather_data, set_weather_data] = useState({
        data: [{time: "00:00:00", temp: 0, humidity: 0}],
        lastFetched: null
    });
    
    const getWeatherDataTest = async (lastFetched) => {
        const sensorDataRef = collection(firestore, "sensor_data");
        let query = sensorDataRef.orderBy("timestamp", "desc").limit(5);
        if (lastFetched) {
            query = query.where("timestamp", ">", lastFetched); //"timestamp should be name of time in firebase"
        }
    
        const snapshot = await getDocs(query);
        const sensorData = snapshot.docs.map((doc) => doc.data());
        console.log(sensorData.length + " data points received");
        
        // Update the lastFetched timestamp
        const newLastFetched = snapshot.docs.length > 0 ? snapshot.docs[snapshot.docs.length - 1].data().timestamp : null;
    
        return { data: sensorData, lastFetched: newLastFetched };
    }

    const test_firestore = async () => {
        const fetchedData = await getWeatherDataTest(weather_data.lastFetched);
        set_weather_data(prevData => ({
            data: [...prevData.data, ...fetchedData.data],
            lastFetched: fetchedData.lastFetched
        }));
    }
    
    useEffect(() => {
        getWeatherDataTest(weather_data.lastFetched).then(fetchedData => {
            set_weather_data(prevData => ({
                data: [...prevData.data, ...fetchedData.data],
                lastFetched: fetchedData.lastFetched
            }));
        });
    }, []);

    //useEffect function that fetches every 10 seconds can be useful later on for updating from firestore when it is running.
    // useEffect(() => {
    //     // Define a function to fetch data and update state
    //     const fetchData = async () => {
    //         const fetchedData = await getWeatherDataTest(weather_data.lastFetched);
    //         set_weather_data(prevData => ({
    //             data: [...prevData.data, ...fetchedData.data],
    //             lastFetched: fetchedData.lastFetched
    //         }));
    //     };
    
    //     // Call it immediately for the initial data fetch
    //     fetchData();
    
    //     // Set up the interval to fetch data every x milliseconds (e.g., 10000 for 10 seconds)
    //     const intervalId = setInterval(fetchData, 10000);
    
    //     // Cleanup the interval on component unmount
    //     return () => clearInterval(intervalId);
    // }, []);



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
                    color: "#3498db",
                    data: weather_data.data.map((entry) => (
                        {date: entry['time'], temp: entry['temp']}
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
                getValue: (datum) => datum.date,
                formatTick: (tickValue) => {
                    // Use a date formatting library to format the tickValue
                    return formattedDate;
                }
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
            <h1>Temperature</h1>
            <ResizableBox style={{ background: "#f4f4f4", padding: "20px" }} height={750} width={750}>
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
