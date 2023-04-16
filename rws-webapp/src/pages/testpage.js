import {useRouter} from 'next/router'

export default function TestPage(){

    const router = useRouter();

    return(
        <>
            <h1>test page title</h1>
            <button onClick={() => router.push('/')}>back home</button>
        </>
    );
}
