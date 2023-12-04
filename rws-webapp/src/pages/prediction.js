import React, { useState } from 'react';
import { firestore } from '@/firebase';
import { collection, getDocs, query, orderBy, limit } from 'firebase/firestore';

const Prediction = () => {
    const [predictions, setPredictions] = useState({ Cedar: null, Fall_Elm: null, Ragweed: null, Weeds: null });
    const [isLoading, setIsLoading] = useState(false);

    // Function to fetch data from Firebase
    const fetchDataFromFirebase = async () => {
        try {
            const sensorDataRef = collection(firestore, "sensor_data");
            const queryRef = query(sensorDataRef, orderBy("time", "desc"), limit(1));
            const snapshot = await getDocs(queryRef);
            const latestEntry = snapshot.docs.map(doc => {
                const data = doc.data();
                return {
                    direction: data.direction,
                    speed: data.speed,
                    humidity: data.humidity,
                    temp: data.temp,
                    pressure: data.pressure,
                    rain: data.rain
                };
            })[0]; // Get the first (latest) entry
            return latestEntry ? latestEntry : null; // Return the entry or null if no data
        } catch (error) {
            console.error("Error fetching data from Firebase:", error);
            return null; // Return null in case of error
        }
    };

    // Function to send data to Python server and get predictions
    const getPredictionsFromPythonServer = async (data) => {
        setIsLoading(true);
        try {
            const response = await fetch('http://localhost:5000/predict', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(data),
            });
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            const predictionResults = await response.json();
            setPredictions(predictionResults); // Set the state with all predictions
        } catch (error) {
            console.error("There was an error fetching the predictions:", error);
        }
        setIsLoading(false);
    };

    // Handler for the button click
    const handleButtonClick = async () => {
        const fetchedData = await fetchDataFromFirebase();
        const formattedData = formatDataForModel(fetchedData);
        await getPredictionsFromPythonServer(formattedData);
    };

    // Function to format data for the model
    const formatDataForModel = (data) => {
        return data;
    };

    return (
        <div>
            <h1>Allergen Prediction</h1>
            <button onClick={handleButtonClick} disabled={isLoading}>
                {isLoading ? 'Loading...' : 'Load and Predict'}
            </button>
            <div>
                <h2>Predictions:</h2>
                {Object.entries(predictions).map(([key, value]) => (
                    <div key={key}>
                        <h3>{key}</h3>
                        <pre>{JSON.stringify(value, null, 2)}</pre>
                    </div>
                ))}
            </div>
        </div>
    );
};

export default Prediction;
