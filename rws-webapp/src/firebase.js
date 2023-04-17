import { initializeApp} from "firebase/app"
import { getFirestore } from "firebase/firestore"

const firebaseConfig = {
    apiKey: "AIzaSyDgiWf5kcxIIcW_dzMzSL8Muc2Z_ASzq18",
    authDomain: "remote-weather-station-31653.firebaseapp.com",
    projectId: "remote-weather-station-31653",
    storageBucket: "remote-weather-station-31653.appspot.com",
    messagingSenderId: "238237789817",
    appId: "1:238237789817:web:5b4648d361126ca5912ea8"
};

const app = initializeApp(firebaseConfig);
const firestore = getFirestore(app);

export { firestore }
