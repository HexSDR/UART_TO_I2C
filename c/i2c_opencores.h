#ifndef __I2C_OPENCORES_H__
#define __I2C_OPENCORES_H__

 

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


void I2C_init(unsigned int  base,unsigned int  clk,unsigned int  speed);
int I2C_start(unsigned int  base, unsigned int  add, unsigned int  read);
unsigned int  I2C_read(unsigned int  base,unsigned int  last);
unsigned int  I2C_write(unsigned int  base,unsigned char   data, unsigned int  last);
#define I2C_OK (0)
#define I2C_ACK (0)
#define I2C_NOACK (1)
#define I2C_ABITRATION_LOST (2)

#define I2C_OPENCORES_INSTANCE(name, dev) extern int alt_no_storage
#define I2C_OPENCORES_INIT(name, dev) while (0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __I2C_OPENCORES_H__ */
