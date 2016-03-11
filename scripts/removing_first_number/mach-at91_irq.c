 1 /*
  2  * linux/arch/arm/mach-at91/irq.c
  3  *
  4  *  Copyright (C) 2004 SAN People
  5  *  Copyright (C) 2004 ATMEL
  6  *  Copyright (C) Rick Bronson
  7  *
  8  * This program is free software; you can redistribute it and/or modify
  9  * it under the terms of the GNU General Public License as published by
 10  * the Free Software Foundation; either version 2 of the License, or
 11  * (at your option) any later version.
 12  *
 13  * This program is distributed in the hope that it will be useful,
 14  * but WITHOUT ANY WARRANTY; without even the implied warranty of
 15  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 16  * GNU General Public License for more details.
 17  *
 18  * You should have received a copy of the GNU General Public License
 19  * along with this program; if not, write to the Free Software
 20  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 21  */
 22 
 23 #include <linux/init.h>
 24 #include <linux/module.h>
 25 #include <linux/mm.h>
 26 #include <linux/bitmap.h>
 27 #include <linux/types.h>
 28 #include <linux/irq.h>
 29 #include <linux/of.h>
 30 #include <linux/of_address.h>
 31 #include <linux/of_irq.h>
 32 #include <linux/irqdomain.h>
 33 #include <linux/err.h>
 34 #include <linux/slab.h>
 35 
 36 #include <mach/hardware.h>
 37 #include <asm/irq.h>
 38 #include <asm/setup.h>
 39 
 40 #include <asm/exception.h>
 41 #include <asm/mach/arch.h>
 42 #include <asm/mach/irq.h>
 43 #include <asm/mach/map.h>
 44 
 45 #include "at91_aic.h"
 46 
 47 void __iomem *at91_aic_base;
 48 static struct irq_domain *at91_aic_domain;
 49 static struct device_node *at91_aic_np;
 50 static unsigned int n_irqs = NR_AIC_IRQS;
 51 static unsigned long at91_aic_caps = 0;
 52 
 53 /* AIC5 introduces a Source Select Register */
 54 #define AT91_AIC_CAP_AIC5       (1 << 0)
 55 #define has_aic5()              (at91_aic_caps & AT91_AIC_CAP_AIC5)
 56 
 57 #ifdef CONFIG_PM
 58 
 59 static unsigned long *wakeups;
 60 static unsigned long *backups;
 61 
 62 #define set_backup(bit) set_bit(bit, backups)
 63 #define clear_backup(bit) clear_bit(bit, backups)
 64 
 65 static int at91_aic_pm_init(void)
 66 {
 67         backups = kzalloc(BITS_TO_LONGS(n_irqs) * sizeof(*backups), GFP_KERNEL);
 68         if (!backups)
 69                 return -ENOMEM;
 70 
 71         wakeups = kzalloc(BITS_TO_LONGS(n_irqs) * sizeof(*backups), GFP_KERNEL);
 72         if (!wakeups) {
 73                 kfree(backups);
 74                 return -ENOMEM;
 75         }
 76 
 77         return 0;
 78 }
 79 
 80 static int at91_aic_set_wake(struct irq_data *d, unsigned value)
 81 {
 82         if (unlikely(d->hwirq >= n_irqs))
 83                 return -EINVAL;
 84 
 85         if (value)
 86                 set_bit(d->hwirq, wakeups);
 87         else
 88                 clear_bit(d->hwirq, wakeups);
 89 
 90         return 0;
 91 }
 92 
 93 void at91_irq_suspend(void)
 94 {
 95         int bit = -1;
 96 
 97         if (has_aic5()) {
 98                 /* disable enabled irqs */
 99                 while ((bit = find_next_bit(backups, n_irqs, bit + 1)) < n_irqs) {
100                         at91_aic_write(AT91_AIC5_SSR,
101                                        bit & AT91_AIC5_INTSEL_MSK);
102                         at91_aic_write(AT91_AIC5_IDCR, 1);
103                 }
104                 /* enable wakeup irqs */
105                 bit = -1;
106                 while ((bit = find_next_bit(wakeups, n_irqs, bit + 1)) < n_irqs) {
107                         at91_aic_write(AT91_AIC5_SSR,
108                                        bit & AT91_AIC5_INTSEL_MSK);
109                         at91_aic_write(AT91_AIC5_IECR, 1);
110                 }
111         } else {
112                 at91_aic_write(AT91_AIC_IDCR, *backups);
113                 at91_aic_write(AT91_AIC_IECR, *wakeups);
114         }
115 }
116 
117 void at91_irq_resume(void)
118 {
119         int bit = -1;
120 
121         if (has_aic5()) {
122                 /* disable wakeup irqs */
123                 while ((bit = find_next_bit(wakeups, n_irqs, bit + 1)) < n_irqs) {
124                         at91_aic_write(AT91_AIC5_SSR,
125                                        bit & AT91_AIC5_INTSEL_MSK);
126                         at91_aic_write(AT91_AIC5_IDCR, 1);
127                 }
128                 /* enable irqs disabled for suspend */
129                 bit = -1;
130                 while ((bit = find_next_bit(backups, n_irqs, bit + 1)) < n_irqs) {
131                         at91_aic_write(AT91_AIC5_SSR,
132                                        bit & AT91_AIC5_INTSEL_MSK);
133                         at91_aic_write(AT91_AIC5_IECR, 1);
134                 }
135         } else {
136                 at91_aic_write(AT91_AIC_IDCR, *wakeups);
137                 at91_aic_write(AT91_AIC_IECR, *backups);
138         }
139 }
140 
141 #else
142 static inline int at91_aic_pm_init(void)
143 {
144         return 0;
145 }
146 
147 #define set_backup(bit)
148 #define clear_backup(bit)
149 #define at91_aic_set_wake       NULL
150 
151 #endif /* CONFIG_PM */
152 
153 asmlinkage void __exception_irq_entry
154 at91_aic_handle_irq(struct pt_regs *regs)
155 {
156         u32 irqnr;
157         u32 irqstat;
158 
159         irqnr = at91_aic_read(AT91_AIC_IVR);
160         irqstat = at91_aic_read(AT91_AIC_ISR);
161 
162         /*
163          * ISR value is 0 when there is no current interrupt or when there is
164          * a spurious interrupt
165          */
166         if (!irqstat)
167                 at91_aic_write(AT91_AIC_EOICR, 0);
168         else
169                 handle_IRQ(irqnr, regs);
170 }
171 
172 asmlinkage void __exception_irq_entry
173 at91_aic5_handle_irq(struct pt_regs *regs)
174 {
175         u32 irqnr;
176         u32 irqstat;
177 
178         irqnr = at91_aic_read(AT91_AIC5_IVR);
179         irqstat = at91_aic_read(AT91_AIC5_ISR);
180 
181         if (!irqstat)
182                 at91_aic_write(AT91_AIC5_EOICR, 0);
183         else
184                 handle_IRQ(irqnr, regs);
185 }
186 
187 static void at91_aic_mask_irq(struct irq_data *d)
188 {
189         /* Disable interrupt on AIC */
190         at91_aic_write(AT91_AIC_IDCR, 1 << d->hwirq);
191         /* Update ISR cache */
192         clear_backup(d->hwirq);
193 }
194 
195 static void __maybe_unused at91_aic5_mask_irq(struct irq_data *d)
196 {
197         /* Disable interrupt on AIC5 */
198         at91_aic_write(AT91_AIC5_SSR, d->hwirq & AT91_AIC5_INTSEL_MSK);
199         at91_aic_write(AT91_AIC5_IDCR, 1);
200         /* Update ISR cache */
201         clear_backup(d->hwirq);
202 }
203 
204 static void at91_aic_unmask_irq(struct irq_data *d)
205 {
206         /* Enable interrupt on AIC */
207         at91_aic_write(AT91_AIC_IECR, 1 << d->hwirq);
208         /* Update ISR cache */
209         set_backup(d->hwirq);
210 }
211 
212 static void __maybe_unused at91_aic5_unmask_irq(struct irq_data *d)
213 {
214         /* Enable interrupt on AIC5 */
215         at91_aic_write(AT91_AIC5_SSR, d->hwirq & AT91_AIC5_INTSEL_MSK);
216         at91_aic_write(AT91_AIC5_IECR, 1);
217         /* Update ISR cache */
218         set_backup(d->hwirq);
219 }
220 
221 static void at91_aic_eoi(struct irq_data *d)
222 {
223         /*
224          * Mark end-of-interrupt on AIC, the controller doesn't care about
225          * the value written. Moreover it's a write-only register.
226          */
227         at91_aic_write(AT91_AIC_EOICR, 0);
228 }
229 
230 static void __maybe_unused at91_aic5_eoi(struct irq_data *d)
231 {
232         at91_aic_write(AT91_AIC5_EOICR, 0);
233 }
234 
235 static unsigned long *at91_extern_irq;
236 
237 u32 at91_get_extern_irq(void)
238 {
239         if (!at91_extern_irq)
240                 return 0;
241         return *at91_extern_irq;
242 }
243 
244 #define is_extern_irq(hwirq) test_bit(hwirq, at91_extern_irq)
245 
246 static int at91_aic_compute_srctype(struct irq_data *d, unsigned type)
247 {
248         int srctype;
249 
250         switch (type) {
251         case IRQ_TYPE_LEVEL_HIGH:
252                 srctype = AT91_AIC_SRCTYPE_HIGH;
253                 break;
254         case IRQ_TYPE_EDGE_RISING:
255                 srctype = AT91_AIC_SRCTYPE_RISING;
256                 break;
257         case IRQ_TYPE_LEVEL_LOW:
258                 if ((d->hwirq == AT91_ID_FIQ) || is_extern_irq(d->hwirq))               /* only supported on external interrupts */
259                         srctype = AT91_AIC_SRCTYPE_LOW;
260                 else
261                         srctype = -EINVAL;
262                 break;
263         case IRQ_TYPE_EDGE_FALLING:
264                 if ((d->hwirq == AT91_ID_FIQ) || is_extern_irq(d->hwirq))               /* only supported on external interrupts */
265                         srctype = AT91_AIC_SRCTYPE_FALLING;
266                 else
267                         srctype = -EINVAL;
268                 break;
269         default:
270                 srctype = -EINVAL;
271         }
272 
273         return srctype;
274 }
275 
276 static int at91_aic_set_type(struct irq_data *d, unsigned type)
277 {
278         unsigned int smr;
279         int srctype;
280 
281         srctype = at91_aic_compute_srctype(d, type);
282         if (srctype < 0)
283                 return srctype;
284 
285         if (has_aic5()) {
286                 at91_aic_write(AT91_AIC5_SSR,
287                                d->hwirq & AT91_AIC5_INTSEL_MSK);
288                 smr = at91_aic_read(AT91_AIC5_SMR) & ~AT91_AIC_SRCTYPE;
289                 at91_aic_write(AT91_AIC5_SMR, smr | srctype);
290         } else {
291                 smr = at91_aic_read(AT91_AIC_SMR(d->hwirq))
292                       & ~AT91_AIC_SRCTYPE;
293                 at91_aic_write(AT91_AIC_SMR(d->hwirq), smr | srctype);
294         }
295 
296         return 0;
297 }
298 
299 static struct irq_chip at91_aic_chip = {
300         .name           = "AIC",
301         .irq_mask       = at91_aic_mask_irq,
302         .irq_unmask     = at91_aic_unmask_irq,
303         .irq_set_type   = at91_aic_set_type,
304         .irq_set_wake   = at91_aic_set_wake,
305         .irq_eoi        = at91_aic_eoi,
306 };
307 
308 static void __init at91_aic_hw_init(unsigned int spu_vector)
309 {
310         int i;
311 
312         /*
313          * Perform 8 End Of Interrupt Command to make sure AIC
314          * will not Lock out nIRQ
315          */
316         for (i = 0; i < 8; i++)
317                 at91_aic_write(AT91_AIC_EOICR, 0);
318 
319         /*
320          * Spurious Interrupt ID in Spurious Vector Register.
321          * When there is no current interrupt, the IRQ Vector Register
322          * reads the value stored in AIC_SPU
323          */
324         at91_aic_write(AT91_AIC_SPU, spu_vector);
325 
326         /* No debugging in AIC: Debug (Protect) Control Register */
327         at91_aic_write(AT91_AIC_DCR, 0);
328 
329         /* Disable and clear all interrupts initially */
330         at91_aic_write(AT91_AIC_IDCR, 0xFFFFFFFF);
331         at91_aic_write(AT91_AIC_ICCR, 0xFFFFFFFF);
332 }
333 
334 static void __init __maybe_unused at91_aic5_hw_init(unsigned int spu_vector)
335 {
336         int i;
337 
338         /*
339          * Perform 8 End Of Interrupt Command to make sure AIC
340          * will not Lock out nIRQ
341          */
342         for (i = 0; i < 8; i++)
343                 at91_aic_write(AT91_AIC5_EOICR, 0);
344 
345         /*
346          * Spurious Interrupt ID in Spurious Vector Register.
347          * When there is no current interrupt, the IRQ Vector Register
348          * reads the value stored in AIC_SPU
349          */
350         at91_aic_write(AT91_AIC5_SPU, spu_vector);
351 
352         /* No debugging in AIC: Debug (Protect) Control Register */
353         at91_aic_write(AT91_AIC5_DCR, 0);
354 
355         /* Disable and clear all interrupts initially */
356         for (i = 0; i < n_irqs; i++) {
357                 at91_aic_write(AT91_AIC5_SSR, i & AT91_AIC5_INTSEL_MSK);
358                 at91_aic_write(AT91_AIC5_IDCR, 1);
359                 at91_aic_write(AT91_AIC5_ICCR, 1);
360         }
361 }
362 
363 #if defined(CONFIG_OF)
364 static unsigned int *at91_aic_irq_priorities;
365 
366 static int at91_aic_irq_map(struct irq_domain *h, unsigned int virq,
367                                                         irq_hw_number_t hw)
368 {
369         /* Put virq number in Source Vector Register */
370         at91_aic_write(AT91_AIC_SVR(hw), virq);
371 
372         /* Active Low interrupt, with priority */
373         at91_aic_write(AT91_AIC_SMR(hw),
374                        AT91_AIC_SRCTYPE_LOW | at91_aic_irq_priorities[hw]);
375 
376         irq_set_chip_and_handler(virq, &at91_aic_chip, handle_fasteoi_irq);
377         set_irq_flags(virq, IRQF_VALID | IRQF_PROBE);
378 
379         return 0;
380 }
381 
382 static int at91_aic5_irq_map(struct irq_domain *h, unsigned int virq,
383                 irq_hw_number_t hw)
384 {
385         at91_aic_write(AT91_AIC5_SSR, hw & AT91_AIC5_INTSEL_MSK);
386 
387         /* Put virq number in Source Vector Register */
388         at91_aic_write(AT91_AIC5_SVR, virq);
389 
390         /* Active Low interrupt, with priority */
391         at91_aic_write(AT91_AIC5_SMR,
392                        AT91_AIC_SRCTYPE_LOW | at91_aic_irq_priorities[hw]);
393 
394         irq_set_chip_and_handler(virq, &at91_aic_chip, handle_fasteoi_irq);
395         set_irq_flags(virq, IRQF_VALID | IRQF_PROBE);
396 
397         return 0;
398 }
399 
400 static int at91_aic_irq_domain_xlate(struct irq_domain *d, struct device_node *ctrlr,
401                                 const u32 *intspec, unsigned int intsize,
402                                 irq_hw_number_t *out_hwirq, unsigned int *out_type)
403 {
404         if (WARN_ON(intsize < 3))
405                 return -EINVAL;
406         if (WARN_ON(intspec[0] >= n_irqs))
407                 return -EINVAL;
408         if (WARN_ON((intspec[2] < AT91_AIC_IRQ_MIN_PRIORITY)
409                     || (intspec[2] > AT91_AIC_IRQ_MAX_PRIORITY)))
410                 return -EINVAL;
411 
412         *out_hwirq = intspec[0];
413         *out_type = intspec[1] & IRQ_TYPE_SENSE_MASK;
414         at91_aic_irq_priorities[*out_hwirq] = intspec[2];
415 
416         return 0;
417 }
418 
419 static struct irq_domain_ops at91_aic_irq_ops = {
420         .map    = at91_aic_irq_map,
421         .xlate  = at91_aic_irq_domain_xlate,
422 };
423 
424 int __init at91_aic_of_common_init(struct device_node *node,
425                                     struct device_node *parent)
426 {
427         struct property *prop;
428         const __be32 *p;
429         u32 val;
430 
431         at91_extern_irq = kzalloc(BITS_TO_LONGS(n_irqs)
432                                   * sizeof(*at91_extern_irq), GFP_KERNEL);
433         if (!at91_extern_irq)
434                 return -ENOMEM;
435 
436         if (at91_aic_pm_init()) {
437                 kfree(at91_extern_irq);
438                 return -ENOMEM;
439         }
440 
441         at91_aic_irq_priorities = kzalloc(n_irqs
442                                           * sizeof(*at91_aic_irq_priorities),
443                                           GFP_KERNEL);
444         if (!at91_aic_irq_priorities)
445                 return -ENOMEM;
446 
447         at91_aic_base = of_iomap(node, 0);
448         at91_aic_np = node;
449 
450         at91_aic_domain = irq_domain_add_linear(at91_aic_np, n_irqs,
451                                                 &at91_aic_irq_ops, NULL);
452         if (!at91_aic_domain)
453                 panic("Unable to add AIC irq domain (DT)\n");
454 
455         of_property_for_each_u32(node, "atmel,external-irqs", prop, p, val) {
456                 if (val >= n_irqs)
457                         pr_warn("AIC: external irq %d >= %d skip it\n",
458                                 val, n_irqs);
459                 else
460                         set_bit(val, at91_extern_irq);
461         }
462 
463         irq_set_default_host(at91_aic_domain);
464 
465         return 0;
466 }
467 
468 int __init at91_aic_of_init(struct device_node *node,
469                                      struct device_node *parent)
470 {
471         int err;
472 
473         err = at91_aic_of_common_init(node, parent);
474         if (err)
475                 return err;
476 
477         at91_aic_hw_init(n_irqs);
478 
479         return 0;
480 }
481 
482 int __init at91_aic5_of_init(struct device_node *node,
483                                      struct device_node *parent)
484 {
485         int err;
486 
487         at91_aic_caps |= AT91_AIC_CAP_AIC5;
488         n_irqs = NR_AIC5_IRQS;
489         at91_aic_chip.irq_ack           = at91_aic5_mask_irq;
490         at91_aic_chip.irq_mask          = at91_aic5_mask_irq;
491         at91_aic_chip.irq_unmask        = at91_aic5_unmask_irq;
492         at91_aic_chip.irq_eoi           = at91_aic5_eoi;
493         at91_aic_irq_ops.map            = at91_aic5_irq_map;
494 
495         err = at91_aic_of_common_init(node, parent);
496         if (err)
497                 return err;
498 
499         at91_aic5_hw_init(n_irqs);
500 
501         return 0;
502 }
503 #endif
504 
505 /*
506  * Initialize the AIC interrupt controller.
507  */
508 void __init at91_aic_init(unsigned int *priority, unsigned int ext_irq_mask)
509 {
510         unsigned int i;
511         int irq_base;
512 
513         at91_extern_irq = kzalloc(BITS_TO_LONGS(n_irqs)
514                                   * sizeof(*at91_extern_irq), GFP_KERNEL);
515 
516         if (at91_aic_pm_init() || at91_extern_irq == NULL)
517                 panic("Unable to allocate bit maps\n");
518 
519         *at91_extern_irq = ext_irq_mask;
520 
521         at91_aic_base = ioremap(AT91_AIC, 512);
522         if (!at91_aic_base)
523                 panic("Unable to ioremap AIC registers\n");
524 
525         /* Add irq domain for AIC */
526         irq_base = irq_alloc_descs(-1, 0, n_irqs, 0);
527         if (irq_base < 0) {
528                 WARN(1, "Cannot allocate irq_descs, assuming pre-allocated\n");
529                 irq_base = 0;
530         }
531         at91_aic_domain = irq_domain_add_legacy(at91_aic_np, n_irqs,
532                                                 irq_base, 0,
533                                                 &irq_domain_simple_ops, NULL);
534 
535         if (!at91_aic_domain)
536                 panic("Unable to add AIC irq domain\n");
537 
538         irq_set_default_host(at91_aic_domain);
539 
540         /*
541          * The IVR is used by macro get_irqnr_and_base to read and verify.
542          * The irq number is NR_AIC_IRQS when a spurious interrupt has occurred.
543          */
544         for (i = 0; i < n_irqs; i++) {
545                 /* Put hardware irq number in Source Vector Register: */
546                 at91_aic_write(AT91_AIC_SVR(i), NR_IRQS_LEGACY + i);
547                 /* Active Low interrupt, with the specified priority */
548                 at91_aic_write(AT91_AIC_SMR(i), AT91_AIC_SRCTYPE_LOW | priority[i]);
549                 irq_set_chip_and_handler(NR_IRQS_LEGACY + i, &at91_aic_chip, handle_fasteoi_irq);
550                 set_irq_flags(i, IRQF_VALID | IRQF_PROBE);
551         }
552 
553         at91_aic_hw_init(n_irqs);
554 }
