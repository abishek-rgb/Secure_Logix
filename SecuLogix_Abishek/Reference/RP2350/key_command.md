##### **Inter-Core Communication Commands (Bare Metal)**



**1**. **multicore\_launch\_core1(function\_name)**

Boot aagum pothu Core 0 mattum thaan run aagum.      Core 1-a thoongitrukkurathula irunthu ezhuppi, athukku oru specific velai (task) kodukka   intha command use **aagum**.



**2. multicore\_fifo\_push\_blocking(data)**

&#x20;Oru core-la irunthu innoru core-ku 32-bit value-a hardware mailbox valiya "push" panna (anuppa) ithu use aagum.



**3.** **multicore\_fifo\_pop\_blocking()**

&#x20;Innoru core push panna antha data-va udane receive panna (padikka) intha command thevai.



##### **Memory Management Commands \& Attributes**



1. **\_\_not\_in\_flash**

Ithu oru function attribute. Ungaloda romba mukkiyaman code-a slow aana external flash-la vekkama, fast-aana internal SRAM-ku maathi run panna ithu use aagum.





**2. sram\_bank0 / .sram\_bank1 (Memory Sections)**

"Bus contention" varama irukka (rendu core-um ore memory-a ore nerathula thedama irukka), Core 0 data-va oru SRAM bank-layum, Core 1 data-va innoru bank-layum pirichu vekka intha memory section variables use aagum.



##### **Hardware Spinlocks (Memory Read/Write Protection)**



1. **spin\_lock\_claim\_unused(true)**

RP2350-la total-a 32 hardware spinlocks irukku. Athula free-a irukka oru lock-a namma code-ku eduthukka ithu use aagum.





**2. spin\_lock() / spin\_lock\_blocking()**

Oru shared memory-a write panrathukku munnadi, athai vera yaarum use pannama irukka hardware-a "lock" (saavi poda) panna ithu use aagum.





**3. spin\_unlock()**

Shared memory-la namma velai mudinjathum, innoru core athai safely access pannika antha memory-a "unlock" panni (saavi eduthu) thara ithu use aagum. Ithu race conditions-a thadukkum.



##### **RTOS Data Transfer Commands (FreeRTOS)**



1. **xQueueSend():**

Rendu tasks illa cores naduvula safe-a data anuppa (FIFO queue method valiya). Ithu bare metal FIFOs-ku software alternative.



**2.** **xQueueReceive():**

Queue-la anuppuna data-va wait panni receive panna.



**3. xMessageBufferSend() / xMessageBufferReceive()**

Rendu task naduvula periya size arrays illa variable length messages-a anuppa ithu useful-a irukkum.



**4. xSemaphoreTake()**

RTOS-la oru shared resource-a (memory/UART) use panrathukku munnadi access "Mutex" saaviya lock panna. Ithu spin\_lock-oda software version.



**5. xSemaphoreGive()**

Work mudinjathum antha resource-a release (unlock) panna.

