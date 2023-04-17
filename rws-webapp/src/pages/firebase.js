import {firebase, initializeApp} from "firebase/app"
// import "firebase/firestore"
import { getFirestore } from "firebase/firestore"

const firebaseConfig = {
    apiKey: "AIzaSyDRkE8ins1WEwifP6jsKiEGj9XIF60Id4A",
    authDomain: "test-esp-api.firebaseapp.com",
    databaseURL: "https://test-esp-api-default-rtdb.firebaseio.com",
    projectId: "test-esp-api",
    storageBucket: "test-esp-api.appspot.com",
    messagingSenderId: "1072158518009",
    appId: "1:1072158518009:web:fbd9c2d687a46d9519d028",
    measurementId: "G-20NX3BL6P0"
};

const app = initializeApp(firebaseConfig);
const firestore = getFirestore(app);

// const app = initializeApp(firebaseConfig);
// firebase.initializeApp(firebaseConfig);
// const firestore = firebase.firestore();



export { firestore }
