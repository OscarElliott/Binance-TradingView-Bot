import * as React from "react"
import * as SliderPrimitive from "@radix-ui/react-slider"

import { cn } from "@/lib/utils"

const CustomSlider = React.forwardRef<
  React.ElementRef<typeof SliderPrimitive.Root>,
  React.ComponentPropsWithoutRef<typeof SliderPrimitive.Root>
>(({ className, ...props }, ref) => {
  const [displayValue, setDisplayValue] = React.useState(props.defaultValue?.[0] || 1)

  // Convert linear slider value (0-100) to custom scale (1-100)
  const toCustomScale = (linearValue: number) => {
    if (linearValue <= 50) {
      return Math.round(1 + (linearValue / 50) * 19)
    } else {
      return Math.round(20 + ((linearValue - 50) / 50) * 80)
    }
  }

  // Convert custom scale value (1-100) to linear slider value (0-100)
  const toLinearScale = (customValue: number) => {
    if (customValue <= 20) {
      return ((customValue - 1) / 19) * 50
    } else {
      return 50 + ((customValue - 20) / 80) * 50
    }
  }

  const handleValueChange = (newValue: number[]) => {
    const customValue = toCustomScale(newValue[0])
    setDisplayValue(customValue)
    if (props.onValueChange) {
      props.onValueChange([customValue])
    }
  }

  return (
    <SliderPrimitive.Root
      ref={ref}
      className={cn(
        "relative flex w-full touch-none select-none items-center",
        className
      )}
      {...props}
      onValueChange={handleValueChange}
      value={[toLinearScale(displayValue)]}
    >
      <SliderPrimitive.Track className="relative h-1.5 w-full grow overflow-hidden rounded-full bg-primary/20">
        <SliderPrimitive.Range className="absolute h-full bg-primary" />
      </SliderPrimitive.Track>
      <SliderPrimitive.Thumb className="block h-4 w-4 rounded-full border border-primary/50 bg-background shadow transition-colors focus-visible:outline-none focus-visible:ring-1 focus-visible:ring-ring disabled:pointer-events-none disabled:opacity-50" />
      <div className="absolute left-0 right-0 mt-1 flex justify-between text-xs text-gray-500">
        <span>1</span>
        <span>20</span>
        <span>100</span>
      </div>
    </SliderPrimitive.Root>
  )
})
CustomSlider.displayName = SliderPrimitive.Root.displayName

export { CustomSlider }