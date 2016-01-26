
extern int Sum(byte count, const byte* data);
extern byte checksum(const byte* data);
extern byte checksum2(const byte* data);
extern float temperature(const byte* data);
       //temperature sur un int multiplier par 10
extern int temperatureint(const byte* data);
extern int temperatureEnt(const byte* data);
extern int temperatureFrac(const byte* data);
extern byte getHumidity(const byte* data);
       // Ne retourne qu'un apercu de l'etat de la baterie : 10 = faible
extern byte battery(const byte* data);
extern byte channel(const byte* data);
extern byte getId (const byte* data);
extern word getSensor (const byte* data);

