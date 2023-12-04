import Head from 'next/head'
import Image from 'next/image'
import { Inter } from 'next/font/google'
import styles from '@/styles/Home.module.css'

import {useRouter} from 'next/router'

const inter = Inter({ subsets: ['latin'] })

export default function Home() {

  const router = useRouter()

  return (
    <>
      <Head>
        <title>Remote Weather Station</title>
        <meta name="description" content="Generated by create next app" />
        <meta name="viewport" content="width=device-width, initial-scale=1" />
        <link rel="icon" href="/favicon.ico" />
      </Head>

      <h1>Remote Weather Station Home</h1>
      <button onClick={() => router.push('/testpage')}>Monitoring</button>
      <button onClick={() => router.push('/history')}>Historical Data</button>
      <button onClick={() => router.push('/prediction')}>Allergen Prediction</button>

    </>
  )
}
