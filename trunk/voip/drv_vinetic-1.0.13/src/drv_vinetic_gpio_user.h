#ifndef _DRV_VINETIC_GPIO_USER_H
#define _DRV_VINETIC_GPIO_USER_H
/****************************************************************************
                  Copyright � 2005  Infineon Technologies AG
                 St. Martin Strasse 53; 81669 Munich, Germany

   THE DELIVERY OF THIS SOFTWARE AS WELL AS THE HEREBY GRANTED NON-EXCLUSIVE, 
   WORLDWIDE LICENSE TO USE, COPY, MODIFY, DISTRIBUTE AND SUBLICENSE THIS
   SOFTWARE IS FREE OF CHARGE.

   THE LICENSED SOFTWARE IS PROVIDED "AS IS" AND INFINEON EXPRESSLY DISCLAIMS 
   ALL REPRESENTATIONS AND WARRANTIES, WHETHER EXPRESS OR IMPLIED, INCLUDING 
   WITHOUT LIMITATION, WARRANTIES OR REPRESENTATIONS OF WORKMANSHIP, 
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, DURABILITY, THAT THE 
   OPERATING OF THE LICENSED SOFTWARE WILL BE ERROR FREE OR FREE OF ANY 
   THIRD PARTY CLAIMS, INCLUDING WITHOUT LIMITATION CLAIMS OF THIRD PARTY 
   INTELLECTUAL PROPERTY INFRINGEMENT. 

   EXCEPT FOR ANY LIABILITY DUE TO WILFUL ACTS OR GROSS NEGLIGENCE AND 
   EXCEPT FOR ANY PERSONAL INJURY INFINEON SHALL IN NO EVENT BE LIABLE FOR 
   ANY CLAIM OR DAMAGES OF ANY KIND, WHETHER IN AN ACTION OF CONTRACT, 
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************/
/** \file drv_vinetic_gpio_user.h VINETIC GPIO/IO resource managment module */

/*****************************************************************************
 * Global Function Definitions
 *****************************************************************************/
IFX_int32_t VINETIC_GpioReserveUser(VINETIC_DEVICE* pDev, VINETIC_IO_GPIO_CONTROL *pCtrl);
IFX_int32_t VINETIC_GpioReleaseUser(VINETIC_DEVICE* pDev, VINETIC_IO_GPIO_CONTROL *pCtrl);
IFX_int32_t VINETIC_GpioConfigUser(VINETIC_DEVICE* pDev, VINETIC_IO_GPIO_CONTROL *pCtrl);
IFX_int32_t VINETIC_GpioSetUser(VINETIC_DEVICE* pDev, VINETIC_IO_GPIO_CONTROL *pCtrl);
IFX_int32_t VINETIC_GpioGetUser(VINETIC_DEVICE* pDev, VINETIC_IO_GPIO_CONTROL *pCtrl);

#endif /* _DRV_VINETIC_GPIO_USER_H */

